//packet length modulation in combined traffic (combined.csv)
// Accuracy 90%
//Original method with one signal packet, 8 packets of shortcodes, and two packets of CRC
//plus header index for signal packet, I prefer this one
//The only difference with packet7 is pretty much RxCallBack

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/wifi-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/applications-module.h"
#include "ns3/command-line.h"
#include "ns3/simulator.h"
#include "ns3/core-module.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <bitset>
#include <queue>
#include <filesystem>
#include <chrono>
#include <iomanip>

double g_duration = 10.0;
std::string g_outBase = "plm_output";
std::string g_runTag = "";
static std::string g_outDir;

using namespace ns3;
namespace fs = std::filesystem;

static std::string
MakeTimeStampedOutDir(const std::string& base, const std::string& tag)
{
  auto now = std::chrono::system_clock::now();
  std::time_t t = std::chrono::system_clock::to_time_t(now);
  std::tm tm{};
#if defined(_WIN32)
  localtime_s(&tm, &t);
#else
  localtime_r(&t, &tm);
#endif
  std::ostringstream ts;
  ts << std::put_time(&tm, "%Y-%m-%d_%H-%M-%S");
  if (!tag.empty()) ts << "_" << tag;

  fs::path dir = fs::path(base) / ts.str();
  fs::create_directories(dir);
  return dir.string();
}

static void
WriteRunMeta(const std::string& outDir)
{
  std::ofstream meta(outDir + "/run_meta.txt", std::ios::out | std::ios::trunc);
  if (!meta) return;
  meta << "duration=" << g_duration << "\n";
  // (Add other flags later if you want)
}



NS_LOG_COMPONENT_DEFINE("PacketLengthModulationWithIndexCycle");

static std::ofstream g_txLog;
static std::ofstream g_rxLog;
static std::ofstream g_decodeLog;
static std::ofstream g_txTraffic;
static std::ofstream g_rxTraffic;

enum ReceiverState {
  RECEIVER_IDLE,
  RECEIVER_DECODING
};

ReceiverState receiverState = RECEIVER_IDLE;
std::vector<uint8_t> receivedChunks;
static uint32_t receivedPackets = 0;
static const uint32_t EXPECTED_DATA_PACKETS = 10;  
static const uint32_t BASE_SIZE = 392;
static const uint32_t INCREMENT = 16;

static uint32_t currentIndex = 999999;

struct TransmittedCodeInfo {
  uint32_t index;      
  uint32_t code;       
  double createTime;   
  double transmitTime; 
};
static std::queue<TransmittedCodeInfo> g_transmittedCodes;

static uint32_t g_codeCounter = 0;

uint8_t ComputeCrc8(uint32_t data)
{
  uint8_t crc = 0x00;
  uint8_t poly = 0x07;
  for (int i = 31; i >= 0; --i)
  {
      uint8_t bit = (data >> i) & 0x01;
      uint8_t msb = (crc >> 7) & 0x01;
      crc = (crc << 1) | bit;
      if (msb) crc ^= poly;
  }
  return crc;
}

void RxCallback(Ptr<const Packet> packet, const Address &address)
{
  double now = Simulator::Now().GetSeconds();
  uint32_t pktSize = packet->GetSize();
  uint32_t indexToLog = 999999;
 
  g_rxTraffic << now << "\t" << pktSize << std::endl;

  // IDLE: look for signal in [61..70].
  if (receiverState == RECEIVER_IDLE)
  {
    if (pktSize >= 32 && pktSize <= 41)
    {
      
      uint32_t idx = pktSize - 32; // in [0..9]
      indexToLog = idx;
      //store that index in "currentIndex"
      NS_LOG_INFO("IDLE => DECODING for codeIndex=" << idx << " at time=" << now);
      receiverState = RECEIVER_DECODING;
      currentIndex = idx;
      receivedPackets = 0;
      receivedChunks.clear();
    }
    else
    {
      NS_LOG_INFO("IDLE: ignoring packet size=" << pktSize << " at time=" << now);
    }

    g_rxLog << indexToLog << "\t" << now << "\t" << pktSize << std::endl;
    return;
  }
  else if (receiverState == RECEIVER_DECODING)
  {
    if (pktSize >= 32 && pktSize <= 41)
    {
      NS_LOG_INFO("DECODING: ignoring new signal size=" << pktSize << " at time=" << now);
      g_rxLog << currentIndex << "\t" << now << "\t" << pktSize << std::endl;
      return;
    }

    indexToLog = currentIndex;
    g_rxLog << indexToLog << "\t" << now << "\t" << pktSize << std::endl;

    if (pktSize < BASE_SIZE || ((pktSize - BASE_SIZE) % INCREMENT) != 0)
    {
    //   NS_LOG_WARN("DECODING: unexpected data packet size => abort decode.");
    //   receiverState = RECEIVER_IDLE;
    //   receivedChunks.clear();
    //   receivedPackets = 0;
    //   currentIndex = 999999;
     NS_LOG_INFO("DECODING: ignoring non-coded packet size=" << pktSize 
                 << " (likely random traffic).");
      return;
    }
    uint8_t nibble = (pktSize - BASE_SIZE) / INCREMENT;
    if (nibble > 15)
    {
      NS_LOG_WARN("DECODING: nibble out of range => abort decode.");
      receiverState = RECEIVER_IDLE;
      receivedChunks.clear();
      receivedPackets=0;
      currentIndex=999999;
      return;
    }
    receivedChunks.push_back(nibble);
    receivedPackets++;
    NS_LOG_INFO("DECODING: codeIndex=" << currentIndex
                 << ", nibble=" << (uint32_t)nibble
                 << ", total=" << receivedPackets);

    if (receivedPackets == EXPECTED_DATA_PACKETS)
    {
      if (receivedChunks.size() < 10)
      {
        NS_LOG_WARN("DECODING: not enough chunks => abort decode.");
        receiverState = RECEIVER_IDLE;
        receivedChunks.clear();
        receivedPackets=0;
        currentIndex=999999;
        return;
      }
      uint32_t shortCode=0;
      for (int i=0; i<8; i++)
      {
        shortCode = (shortCode << 4) | receivedChunks[i];
      }
      uint8_t rCrc = ((receivedChunks[8] << 4) | receivedChunks[9]);
      uint8_t cCrc = ComputeCrc8(shortCode);

      if (!g_transmittedCodes.empty())
      {
        TransmittedCodeInfo info = g_transmittedCodes.front();
        g_transmittedCodes.pop();

        std::string cat = (cCrc == rCrc) ? "A" : "B";

        g_decodeLog << info.index << "\t" << now << "\t" << cat
                    << "\tOriginalSC=" << std::bitset<32>(info.code)
                    << "\tReceivedSC=" << std::bitset<32>(shortCode)
                    << "\tCompCRC=" << std::bitset<8>(cCrc)
                    << "\tRecvCRC=" << std::bitset<8>(rCrc)
                    << std::endl;
        NS_LOG_INFO("DECODE: codeIndex=" << info.index
                     << " => " << cat);
      }
      else
      {
        NS_LOG_WARN("DECODE: no code in queue => out of sync!");
      }

      // reset
      receiverState = RECEIVER_IDLE;
      receivedChunks.clear();
      receivedPackets=0;
      currentIndex=999999;
    }
  }
}

class PacketLengthModulation : public Application
{
public:
    static TypeId GetTypeId(void);
    PacketLengthModulation();
    virtual ~PacketLengthModulation();

    void Setup(Address address, double codeIntervalSeconds);

private:
    virtual void StartApplication(void);
    virtual void StopApplication(void);

    void GenerateAndSendShortCode();
    void SendPacket(uint32_t size, uint32_t codeIndex, bool isSignal);

    Ptr<Socket> m_socket;
    Address m_peer;
    Time m_interval; 
    bool m_running;

    double m_codeIntervalSeconds; 
};

NS_OBJECT_ENSURE_REGISTERED(PacketLengthModulation);

TypeId
PacketLengthModulation::GetTypeId(void)
{
    static TypeId tid = TypeId("ns3::PacketLengthModulation")
        .SetParent<Application>()
        .SetGroupName("Applications")
        .AddConstructor<PacketLengthModulation>();
    return tid;
}

PacketLengthModulation::PacketLengthModulation()
    : m_socket(0),
      m_peer(),
      m_interval(Seconds(0.1)), 
      m_running(false),
      m_codeIntervalSeconds(2)
{
}

PacketLengthModulation::~PacketLengthModulation()
{
    m_socket = 0;
}

void
PacketLengthModulation::Setup(Address address, double codeIntervalSeconds)
{
    m_peer = address;
    m_codeIntervalSeconds = codeIntervalSeconds;
}

void
PacketLengthModulation::StartApplication(void)
{
    m_running = true;
    m_socket = Socket::CreateSocket(GetNode(), UdpSocketFactory::GetTypeId());
    m_socket->Bind();
    m_socket->Connect(m_peer);

    GenerateAndSendShortCode();
}

void
PacketLengthModulation::GenerateAndSendShortCode()
{
    if (!m_running) return;

    double now = Simulator::Now().GetSeconds();

    uint32_t shortCode = rand();

    uint32_t codeIndex = g_codeCounter % 10;
    g_codeCounter++;

    TransmittedCodeInfo info;
    info.index = codeIndex;
    info.code  = shortCode;
    info.createTime = now;
    info.transmitTime = now;
    g_transmittedCodes.push(info);

    uint32_t signalSize = 32 + codeIndex;
    uint8_t crc8 = ComputeCrc8(shortCode);

    std::vector<uint8_t> codeNibbles;
    for (int i=7; i>=0; i--)
    {
       uint8_t nib = (shortCode >> (i*4)) & 0xF;
       codeNibbles.push_back(nib);
    }
    uint8_t hi = (crc8 >> 4) & 0xF;
    uint8_t lo = crc8 & 0xF;
    codeNibbles.push_back(hi);
    codeNibbles.push_back(lo);

    Simulator::Schedule(Seconds(0.0),
                        &PacketLengthModulation::SendPacket,
                        this,
                        signalSize,
                        codeIndex,
                        true);

    for (int i=0; i<(int)codeNibbles.size(); i++)
    {
       uint8_t nib = codeNibbles[i];
       uint32_t dataSize = 392 + (nib * 16);

       double schedOffset = (i+1)*m_interval.GetSeconds(); 
       Simulator::Schedule(Seconds(schedOffset),
                           &PacketLengthModulation::SendPacket,
                           this,
                           dataSize,
                           codeIndex,
                           false);
    }

    Simulator::Schedule(Seconds(m_codeIntervalSeconds),
                        &PacketLengthModulation::GenerateAndSendShortCode,
                        this);
}

void
PacketLengthModulation::SendPacket(uint32_t size, uint32_t codeIndex, bool isSignal)
{
    if (!m_socket || !m_running) return;
    Ptr<Packet> packet = Create<Packet>(size);

    double now = Simulator::Now().GetSeconds();
    g_txLog << codeIndex << "\t" << now << "\t" << size << std::endl;

    g_txTraffic << now << "\t" << size << std::endl;

    int actual = m_socket->Send(packet);
    if (actual > 0)
    {
        NS_LOG_INFO("SendPacket: codeIndex=" << codeIndex
                     << ", isSignal=" << isSignal
                     << ", size=" << size
                     << ", time=" << now);
    }
    else
    {
        NS_LOG_ERROR("SendPacket: failed at time=" << now);
    }
}

void
PacketLengthModulation::StopApplication(void)
{
    m_running = false;
    if (m_socket)
    {
        m_socket->Close();
    }
}

struct PacketInfo
{
    double time;
    uint32_t size;
};

std::vector<PacketInfo> ReadCsvFile(const std::string &filename)
{
    std::vector<PacketInfo> packetInfos;

    std::ifstream file(filename);
    if (!file.is_open())
    {
        NS_LOG_ERROR("Failed to open file " << filename);
        return packetInfos;
    }
    else
    {
        NS_LOG_INFO("Successfully opened file " << filename);
    }

    std::string line;
    std::getline(file, line);

    while (std::getline(file, line))
    {
        std::istringstream ss(line);
        std::string field;
        std::vector<std::string> fields;

        while (std::getline(ss, field, ','))
        {
            if (field.size() >= 2 && field.front() == '"' && field.back() == '"')
            {
                field = field.substr(1, field.size() - 2);
            }
            fields.push_back(field);
        }

        if (fields.size() >= 6)
        {
            try
            {
                double time = std::stod(fields[1]);     
                uint32_t size = std::stoul(fields[5]);  
                packetInfos.push_back({time, size});
            }
            catch (const std::exception &e)
            {
                NS_LOG_WARN("Error parsing line: " << line << " - " << e.what());
            }
        }
        else
        {
            NS_LOG_WARN("Invalid line format: " << line);
        }
    }

    file.close();
    return packetInfos;
}


void SendPacket(Ptr<Socket> socket, uint32_t size)
{
    double now = Simulator::Now().GetSeconds();
    // NS_LOG_INFO("SendPacket background: at " << now 
    //              << " size=" << size 
    //              << " node=" << socket->GetNode()->GetId());
    g_txTraffic << now << "\t" << size << std::endl;
    socket->Send(Create<Packet>(size));
}

void ScheduleBackgroundTraffic(Ptr<Node> node, const std::vector<PacketInfo> &packetInfos, Address remoteAddress)
{
    TypeId tid = TypeId::LookupByName("ns3::UdpSocketFactory");
    Ptr<Socket> socket = Socket::CreateSocket(node, tid);
    socket->SetAllowBroadcast(true);

    socket->Connect(remoteAddress);

    for (const auto &pktInfo : packetInfos)
    {
        Simulator::Schedule(Seconds(pktInfo.time), &SendPacket, socket, pktInfo.size);
    }
}

static void StampLogHeader(std::ofstream& f, const char* name) {
  if (!f) return;
  f << "# " << name << "\n";
  f << "# duration_requested_s=" << g_duration << "\n";
  f << "# start_now_s=" << ns3::Simulator::Now().GetSeconds() << "\n";
  f << "# -----------------------------------------\n";
  f.flush();
}


int main(int argc, char* argv[])
{
    Time::SetResolution(Time::NS);
    LogComponentEnable("PacketLengthModulationWithIndexCycle", LOG_LEVEL_INFO);
    LogComponentEnable("PacketSink", LOG_LEVEL_INFO);

    ns3::CommandLine cmd;
    cmd.AddValue("duration", "Total application run time in seconds", g_duration);
    cmd.AddValue("outBase",  "Base folder for outputs (default: plm_output)", g_outBase);
    cmd.AddValue("tag",      "Optional suffix for this run's folder name",    g_runTag);
    cmd.Parse(argc, argv);

    if (g_duration <= 0.0)
    {
      NS_FATAL_ERROR("duration must be > 0 seconds");
    }

    if (g_duration <= 0.0) {
      NS_FATAL_ERROR("duration must be > 0 seconds");
    }

    // Create timestamped folder and write meta
    g_outDir = MakeTimeStampedOutDir(g_outBase, g_runTag);
    std::cout << "[plm] Output directory: " << g_outDir << std::endl;
    WriteRunMeta(g_outDir);

    // Open your existing logs *inside this folder*, keeping the same filenames
    g_txLog.open( (fs::path(g_outDir) / "tx_sent.txt").string(), std::ios::out | std::ios::trunc );
    StampLogHeader(g_txLog, "TX Sent Log");
    g_rxLog.open( (fs::path(g_outDir) / "rx_received.txt").string(), std::ios::out | std::ios::trunc );
    StampLogHeader(g_rxLog, "RX Sent Log");
    g_decodeLog.open( (fs::path(g_outDir) / "decoded_summary.txt").string(), std::ios::out | std::ios::trunc );
    StampLogHeader(g_decodeLog, "Decoded Summary");
    g_txTraffic.open( (fs::path(g_outDir) / "tx_traffic.txt").string(), std::ios::out | std::ios::trunc );
    StampLogHeader(g_txTraffic, "TX Trafiic");
    g_rxTraffic.open( (fs::path(g_outDir) / "rx_traffic.txt").string(), std::ios::out | std::ios::trunc );
    StampLogHeader(g_rxTraffic, "RX Traffic");

    NodeContainer nodes;
    nodes.Create(2);

    // WiFi stuff
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
    ipv4.SetBase("10.1.1.0","255.255.255.0");
    Ipv4InterfaceContainer ifaces = ipv4.Assign(devices);

    std::srand(time(0));

    Ptr<PacketLengthModulation> modApp = CreateObject<PacketLengthModulation>();
    InetSocketAddress remote = InetSocketAddress(ifaces.GetAddress(1), 8081);
    double codeIntervalSeconds=2;
    modApp->Setup(remote, codeIntervalSeconds);
    nodes.Get(0)->AddApplication(modApp);
    modApp->SetStartTime(Seconds(0.1));
    modApp->SetStopTime(Seconds(g_duration));

    PacketSinkHelper sinkHelper("ns3::UdpSocketFactory",
                                InetSocketAddress(Ipv4Address::GetAny(),8081));
    ApplicationContainer sinks = sinkHelper.Install(nodes.Get(1));
    sinks.Start(Seconds(0.0));
    sinks.Stop(Seconds(g_duration));

    Config::ConnectWithoutContext("/NodeList/1/ApplicationList/*/$ns3::PacketSink/Rx", MakeCallback(&RxCallback));

    NodeContainer bgNodes;
    bgNodes.Create(1);
    NodeContainer allNodes = NodeContainer(nodes, bgNodes);

    NetDeviceContainer bgDevs = wifi.Install(wifiPhy, wifiMac, bgNodes);
    mobility.Install(bgNodes);
    internet.Install(bgNodes);
    ipv4.NewNetwork();
    ipv4.Assign(bgDevs);

    std::vector<PacketInfo> packetInfos = ReadCsvFile("combined.csv"); 
    Address remoteAddressBackground(InetSocketAddress(Ipv4Address("255.255.255.255"), 8081)); 

    for (uint32_t i = 0; i < bgNodes.GetN(); ++i)
    {
        ScheduleBackgroundTraffic(bgNodes.Get(i), packetInfos, remoteAddressBackground);
    }
    Simulator::Stop(Seconds(g_duration+1.0));
    Simulator::Run();

    double sim_end = ns3::Simulator::Now().GetSeconds();
    auto stampEnd = [&](std::ofstream& f){
      if (f) {
        f << "\n# -----------------------------------------\n";
        f << "# sim_end_time_s=" << sim_end << "\n";
        f << "# duration_requested_s=" << g_duration << "\n";
        f.flush();
      }
    };
    stampEnd(g_txLog); stampEnd(g_rxLog); stampEnd(g_decodeLog);
    stampEnd(g_txTraffic); stampEnd(g_rxTraffic);

    Simulator::Destroy();

    if(g_txLog.is_open()) g_txLog.close();
    if(g_rxLog.is_open()) g_rxLog.close();
    if(g_decodeLog.is_open()) g_decodeLog.close();
    if (g_txTraffic.is_open()) g_txTraffic.close();
    if (g_rxTraffic.is_open()) g_rxTraffic.close();

    return 0;
}
