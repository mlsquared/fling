//packet length modulation without traffic
//32 bit short code into 8 packets with CSC-8

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/wifi-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/applications-module.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <ctime>    // For time()
#include <cstdlib>  // For srand() and rand()
#include <bitset>   // For std::bitset

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("PacketLengthModulationWithCRC");

std::ofstream txStream("tx_traffic.txt", std::ios::out);
std::ofstream rxStream("rx_traffic.txt", std::ios::out);

uint32_t receivedPackets = 0;
std::vector<uint8_t> receivedChunks;

uint8_t ComputeCrc8(uint32_t data)
{
  uint8_t crc = 0x00;
  uint8_t poly = 0x07;

  for (int i = 31; i >= 0; --i)
  {
    uint8_t bit = (data >> i) & 0x01;
    uint8_t msb = (crc >> 7) & 0x01;
    crc <<= 1;
    if (msb ^ bit)
      crc ^= poly;
  }
  return crc;
}


void TxCallback(Ptr<const Packet> packet)
{
  txStream << Simulator::Now().GetSeconds() << "\t" << packet->GetSize() << std::endl;
  NS_LOG_INFO("Transmitted packet of size " << packet->GetSize() << " bytes at " << Simulator::Now().GetSeconds() << "s");
}

void RxCallback(Ptr<const Packet> packet, const Address &address)
{
  rxStream << Simulator::Now().GetSeconds() << "\t" << packet->GetSize() << std::endl;
  NS_LOG_INFO("Received packet of size " << packet->GetSize() << " bytes at " << Simulator::Now().GetSeconds() << "s");

  // Decode the chunk value from packet size
  uint32_t pktSize = packet->GetSize();
  uint32_t baseSize = 392;
  uint32_t increment = 16;

  if (pktSize < baseSize || (pktSize - baseSize) % increment != 0)
  {
    NS_LOG_WARN("Received packet with unexpected size: " << pktSize);
    return;
  }

  uint8_t chunkValue = (pktSize - baseSize) / increment;
  if (chunkValue > 15)
  {
    NS_LOG_WARN("Chunk value out of range: " << (uint32_t)chunkValue);
    return;
  }

  receivedChunks.push_back(chunkValue);
  receivedPackets++;

  if (receivedPackets == 10)
  {
    uint32_t receivedShortCode = 0;
    for (size_t i = 0; i < 8; ++i)
    {
      receivedShortCode = (receivedShortCode << 4) | receivedChunks[i];
    }
    uint8_t receivedCrc = 0;
    for (size_t i = 8; i < 10; ++i)
    {
      receivedCrc = (receivedCrc << 4) | receivedChunks[i];
    }

    uint8_t computedCrc = ComputeCrc8(receivedShortCode);

    std::ofstream decodedStream("decoded_short_code.txt", std::ios::out);
    if (computedCrc == receivedCrc)
    {
      NS_LOG_INFO("CRC check passed.");
      NS_LOG_INFO("Reconstructed Short Code: " << std::bitset<32>(receivedShortCode));
      decodedStream << "Received Short Code: " << std::bitset<32>(receivedShortCode) << std::endl;
    }
    else
    {
      NS_LOG_WARN("CRC check failed.");
      decodedStream << "CRC check failed. Data may be corrupted." << std::endl;
      decodedStream << "Received Short Code: " << std::bitset<32>(receivedShortCode) << std::endl;
    }
    decodedStream.close();

    receivedPackets = 0;
    receivedChunks.clear();
  }
}


//PacketLengthModulation class
class PacketLengthModulation : public Application
{
public:
  static TypeId GetTypeId(void);

  PacketLengthModulation();
  virtual ~PacketLengthModulation();

  void Setup(Address address, std::vector<uint32_t> packetSizes, Time interval);

private:
  virtual void StartApplication(void);
  virtual void StopApplication(void);

  void SendPacket(void);
  void ScheduleNextPacket(void);

  Ptr<Socket> m_socket;
  Address m_peer;
  std::vector<uint32_t> m_packetSizes;
  uint32_t m_currentPacket;
  Time m_interval;
  EventId m_sendEvent;
  bool m_running;

  TracedCallback<Ptr<const Packet>> m_txTrace;
};

NS_OBJECT_ENSURE_REGISTERED(PacketLengthModulation);

TypeId
PacketLengthModulation::GetTypeId(void)
{
  static TypeId tid = TypeId("ns3::PacketLengthModulation")
    .SetParent<Application>()
    .SetGroupName("Applications")
    .AddConstructor<PacketLengthModulation>()
    .AddTraceSource("Tx",
                    "A new packet is created and is sent",
                    MakeTraceSourceAccessor(&PacketLengthModulation::m_txTrace),
                    "ns3::Packet::TracedCallback");
  return tid;
}

PacketLengthModulation::PacketLengthModulation()
  : m_socket(0),
    m_peer(),
    m_currentPacket(0),
    m_interval(Seconds(0.1)),
    m_sendEvent(),
    m_running(false)
{
}

PacketLengthModulation::~PacketLengthModulation()
{
  m_socket = 0;
}

void
PacketLengthModulation::Setup(Address address, std::vector<uint32_t> packetSizes, Time interval)
{
  m_peer = address;
  m_packetSizes = packetSizes;
  m_interval = interval;
}

void
PacketLengthModulation::StartApplication(void)
{
  m_running = true;
  m_currentPacket = 0;

  m_socket = Socket::CreateSocket(GetNode(), UdpSocketFactory::GetTypeId());
  m_socket->Bind();
  m_socket->Connect(m_peer);

  SendPacket();
}

void
PacketLengthModulation::StopApplication(void)
{
  m_running = false;

  if (m_sendEvent.IsRunning())
  {
    Simulator::Cancel(m_sendEvent);
  }

  if (m_socket)
  {
    m_socket->Close();
  }
}

void
PacketLengthModulation::SendPacket(void)
{
  if (m_currentPacket < m_packetSizes.size())
  {
    uint32_t pktSize = m_packetSizes[m_currentPacket];
    Ptr<Packet> packet = Create<Packet>(pktSize);

    int actual = m_socket->Send(packet);
    if (actual > 0)
    {
      m_txTrace(packet);
      NS_LOG_INFO("Sent packet " << m_currentPacket + 1 << "/" << m_packetSizes.size()
                   << " of size " << pktSize << " bytes at " << Simulator::Now().GetSeconds() << "s");
    }
    else
    {
      NS_LOG_ERROR("Failed to send packet at " << Simulator::Now().GetSeconds() << "s");
    }

    m_currentPacket++;
    ScheduleNextPacket();
  }
}

void
PacketLengthModulation::ScheduleNextPacket(void)
{
  if (m_running && m_currentPacket < m_packetSizes.size())
  {
    m_sendEvent = Simulator::Schedule(m_interval, &PacketLengthModulation::SendPacket, this);
  }
}

int main(int argc, char *argv[])
{
  Time::SetResolution(Time::NS);

  LogComponentEnable("PacketLengthModulationWithCRC", LOG_LEVEL_INFO);
  LogComponentEnable("PacketSink", LOG_LEVEL_INFO);

  NodeContainer nodes;
  nodes.Create(2);

  WifiHelper wifi;
  wifi.SetStandard(WIFI_STANDARD_80211g);
  wifi.SetRemoteStationManager("ns3::ConstantRateWifiManager",
                               "DataMode", StringValue("ErpOfdmRate54Mbps"),
                               "ControlMode", StringValue("ErpOfdmRate6Mbps"));

  YansWifiPhyHelper wifiPhy;
  YansWifiChannelHelper wifiChannel = YansWifiChannelHelper::Default();
  wifiPhy.SetChannel(wifiChannel.Create());

  WifiMacHelper wifiMac;
  wifiMac.SetType("ns3::AdhocWifiMac");

  NetDeviceContainer devices = wifi.Install(wifiPhy, wifiMac, nodes);

  MobilityHelper mobility;
  mobility.SetPositionAllocator("ns3::GridPositionAllocator",
                                "MinX", DoubleValue(0.0),
                                "MinY", DoubleValue(0.0),
                                "DeltaX", DoubleValue(5.0),
                                "DeltaY", DoubleValue(0.0),
                                "GridWidth", UintegerValue(2),
                                "LayoutType", StringValue("RowFirst"));

  mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
  mobility.Install(nodes);

  InternetStackHelper internet;
  internet.Install(nodes);

  Ipv4AddressHelper ipv4;
  ipv4.SetBase("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer interfaces = ipv4.Assign(devices);

  //////////////////// Generating and Sending Short Code with CRC ///////////////////////////

  // Generate a random 32-bit short code
  srand(time(0));
  uint32_t shortCode = rand(); // Generate random 32-bit integer
  std::cout << "Generated 32-bit Short Code: " << std::bitset<32>(shortCode) << std::endl;

  // Compute CRC-8
  uint8_t crc8 = ComputeCrc8(shortCode);
  std::cout << "Computed CRC-8: " << std::bitset<8>(crc8) << std::endl;

  // Divide the short code into 8 chunks of 4 bits
  std::vector<uint8_t> fourBitChunks;
  for (int i = 7; i >= 0; --i)
  {
    uint8_t chunk = (shortCode >> (i * 4)) & 0xF; // Extract 4 bits
    fourBitChunks.push_back(chunk);
  }

  // Divide the CRC-8 into 2 chunks of 4 bits
  std::vector<uint8_t> crcChunks;
  crcChunks.push_back((crc8 >> 4) & 0xF); 
  crcChunks.push_back(crc8 & 0xF);       

  // Combine the chunks
  std::vector<uint8_t> allChunks = fourBitChunks;
  allChunks.insert(allChunks.end(), crcChunks.begin(), crcChunks.end());

  std::vector<uint32_t> packetSizes;
  uint32_t baseSize = 392;
  uint32_t increment = 16; 

  for (auto chunk : allChunks)
  {
    uint32_t pktSize = baseSize + (chunk * increment); // Packet size ranges from 392 to 632 bytes
    packetSizes.push_back(pktSize);
    std::cout << "Chunk value: " << (uint32_t)chunk << ", Packet size: " << pktSize << " bytes" << std::endl;
  }

  Ptr<PacketLengthModulation> modApp = CreateObject<PacketLengthModulation>();

  Time packetInterval = MilliSeconds(200); 

  InetSocketAddress remote = InetSocketAddress(interfaces.GetAddress(1), 8080);
  modApp->Setup(remote, packetSizes, packetInterval);
  nodes.Get(0)->AddApplication(modApp);

  modApp->SetStartTime(Seconds(0.1));
  modApp->SetStopTime(Seconds(5.0));

  uint16_t sinkPort = 8080;
  Address sinkAddress(InetSocketAddress(Ipv4Address::GetAny(), sinkPort));
  PacketSinkHelper packetSinkHelper("ns3::UdpSocketFactory", sinkAddress);
  ApplicationContainer sinkApps = packetSinkHelper.Install(nodes.Get(1));
  sinkApps.Start(Seconds(0.0));
  sinkApps.Stop(Seconds(5.0));

  Config::ConnectWithoutContext("/NodeList/0/ApplicationList/*/$ns3::PacketLengthModulation/Tx", MakeCallback(&TxCallback));
  Config::ConnectWithoutContext("/NodeList/1/ApplicationList/*/$ns3::PacketSink/Rx", MakeCallback(&RxCallback));

  // Run the simulation
  Simulator::Stop(Seconds(6.0));
  Simulator::Run();
  Simulator::Destroy();

  txStream.close();
  rxStream.close();

  return 0;
}
