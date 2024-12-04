//packet length modulation in combine traffic
//ran for 40 minutes

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/wifi-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/applications-module.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <ctime>    
#include <cstdlib>  
#include <bitset>   
using namespace ns3;

NS_LOG_COMPONENT_DEFINE("PacketLengthModulationWithCRCCorrectedPortSeparated");

std::ofstream txStream("tx_traffic.txt", std::ios::out);
std::ofstream rxStream("rx_traffic.txt", std::ios::out);

enum ReceiverState
{
    RECEIVER_IDLE,
    RECEIVER_DECODING
};

ReceiverState receiverState = RECEIVER_IDLE;
uint32_t receivedPackets = 0;
std::vector<uint8_t> receivedChunks;

std::ofstream decodedResultsStream("decoded_results_summary10.txt", std::ios::out);
uint32_t totalReceivedCodes = 0;
uint32_t totalPassedCrc = 0;
uint32_t totalFailedCrc = 0;

uint8_t ComputeCrc8(uint32_t data)
{
    uint8_t crc = 0x00;
    uint8_t poly = 0x07;

    for (int i = 31; i >= 0; --i)
    {
        uint8_t bit = (data >> i) & 0x01;
        uint8_t msb = (crc >> 7) & 0x01;
        crc = (crc << 1) | bit;
        if (msb)
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

    uint32_t pktSize = packet->GetSize();

    if (receiverState == RECEIVER_IDLE)
    {
        if (pktSize == 61) 
        {
            NS_LOG_INFO("Signal packet received. Starting to decode the message.");
            receiverState = RECEIVER_DECODING;
            receivedPackets = 0;
            receivedChunks.clear();
        }
        return;
    }

    if (receiverState == RECEIVER_DECODING)
    {
        uint32_t baseSize = 392;
        uint32_t increment = 16;

        if (pktSize < baseSize || (pktSize - baseSize) % increment != 0)
        {
            NS_LOG_WARN("Unexpected packet size: " << pktSize << ". Decoding aborted.");
            receiverState = RECEIVER_IDLE;
            receivedPackets = 0;
            receivedChunks.clear();
            return;
        }

        uint8_t chunkValue = (pktSize - baseSize) / increment;
        if (chunkValue > 15)
        {
            NS_LOG_WARN("Chunk value out of range: " << (uint32_t)chunkValue << ". Decoding aborted.");
            receiverState = RECEIVER_IDLE;
            receivedPackets = 0;
            receivedChunks.clear();
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

            totalReceivedCodes++;

            if (computedCrc == receivedCrc)
            {
                totalPassedCrc++;
                NS_LOG_INFO("CRC check passed. Reconstructed Short Code: " << std::bitset<32>(receivedShortCode));
                decodedResultsStream << Simulator::Now().GetSeconds() << "\tPASS\t" << std::bitset<32>(receivedShortCode) << std::endl;
            }
            else
            {
                totalFailedCrc++;
                NS_LOG_WARN("CRC check failed. Computed CRC: " << std::bitset<8>(computedCrc) << ", Received CRC: " << std::bitset<8>(receivedCrc));
                decodedResultsStream << Simulator::Now().GetSeconds() << "\tFAIL\t" << std::bitset<32>(receivedShortCode) << std::endl;
            }

            receiverState = RECEIVER_IDLE;
            receivedPackets = 0;
            receivedChunks.clear();
        }
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
    void SendPacket(uint32_t size);

    Ptr<Socket> m_socket;
    Address m_peer;
    Time m_interval; 
    bool m_running;

    TracedCallback<Ptr<const Packet>> m_txTrace;

    uint32_t totalCodesSent;
    uint32_t totalCodesReceived;
    uint32_t totalCrcPassed;
    uint32_t totalCrcFailed;

    std::ofstream decodedStream; 
    double m_codeIntervalSeconds; 
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
      m_interval(Seconds(0.2)), 
      totalCodesSent(0),
      totalCodesReceived(0),
      totalCrcPassed(0),
      totalCrcFailed(0),
      m_codeIntervalSeconds(10.0) 
{
    decodedStream.open("decoded_results.txt", std::ios::out);
    if (!decodedStream.is_open())
    {
        NS_LOG_ERROR("Failed to open decoded_results.txt for writing.");
    }
}

PacketLengthModulation::~PacketLengthModulation()
{
    if (decodedStream.is_open())
    {
        decodedStream << "Total Codes Sent: " << totalCodesSent << std::endl;
        decodedStream << "Total Codes Received: " << totalCodesReceived << std::endl;
        decodedStream << "Total CRC Passed: " << totalCrcPassed << std::endl;
        decodedStream << "Total CRC Failed: " << totalCrcFailed << std::endl;
        decodedStream.close();
    }
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
    if (!m_running)
        return;

    // Generate a random 32-bit short code
    uint32_t shortCode = rand(); // Generate random 32-bit integer
    NS_LOG_INFO("Generated 32-bit Short Code: " << std::bitset<32>(shortCode));

    // Compute CRC-8
    uint8_t crc8 = ComputeCrc8(shortCode);
    NS_LOG_INFO("Computed CRC-8: " << std::bitset<8>(crc8));

    // Divide the short code into 8 chunks of 4 bits
    std::vector<uint8_t> fourBitChunks;
    for (int i = 7; i >= 0; --i)
    {
        uint8_t chunk = (shortCode >> (i * 4)) & 0xF; // Extract 4 bits
        fourBitChunks.push_back(chunk);
    }

    std::vector<uint8_t> crcChunks;
    crcChunks.push_back((crc8 >> 4) & 0xF);
    crcChunks.push_back(crc8 & 0xF);        

    std::vector<uint8_t> allChunks = fourBitChunks;
    allChunks.insert(allChunks.end(), crcChunks.begin(), crcChunks.end());

    std::vector<uint32_t> packetSizes;
    uint32_t baseSize = 392;
    uint32_t increment = 16;
    uint32_t signalPacketSize = 61; 
    packetSizes.push_back(signalPacketSize);

    Time packetInterval = m_interval; 

    for (auto chunk : allChunks)
    {
        uint32_t pktSize = baseSize + (chunk * increment); 
        packetSizes.push_back(pktSize);
        NS_LOG_INFO("Chunk value: " << (uint32_t)chunk << ", Packet size: " << pktSize << " bytes");
    }

    totalCodesSent++;

    for (size_t i = 0; i < packetSizes.size(); ++i)
    {
        if (i == 0)
        {
            Simulator::Schedule(Seconds(0.0), &PacketLengthModulation::SendPacket, this, packetSizes[i]);
        }
        else
        {
            Simulator::Schedule(m_interval * i, &PacketLengthModulation::SendPacket, this, packetSizes[i]);
        }
    }

    Simulator::Schedule(Seconds(m_codeIntervalSeconds), &PacketLengthModulation::GenerateAndSendShortCode, this);
}

void
PacketLengthModulation::SendPacket(uint32_t size)
{
    if (m_socket && m_running)
    {
        Ptr<Packet> packet = Create<Packet>(size);
        int actual = m_socket->Send(packet);
        if (actual > 0)
        {
            m_txTrace(packet);
            NS_LOG_INFO("Sent packet of size " << size << " bytes at " << Simulator::Now().GetSeconds() << "s");
        }
        else
        {
            NS_LOG_ERROR("Failed to send packet at " << Simulator::Now().GetSeconds() << "s");
        }
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

int main(int argc, char *argv[])
{
    Time::SetResolution(Time::NS);

    LogComponentEnable("PacketLengthModulationWithCRCCorrectedPortSeparated", LOG_LEVEL_INFO);
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

    srand(time(0));

    Ptr<PacketLengthModulation> modApp = CreateObject<PacketLengthModulation>();

    InetSocketAddress remote = InetSocketAddress(interfaces.GetAddress(1), 8080);
    double codeIntervalSeconds = 2.5; 
    modApp->Setup(remote, codeIntervalSeconds);
    nodes.Get(0)->AddApplication(modApp);

    modApp->SetStartTime(Seconds(0.1));
    modApp->SetStopTime(Seconds(2500.0)); 
    uint16_t sinkPort = 8080;
    Address sinkAddress(InetSocketAddress(Ipv4Address::GetAny(), sinkPort));
    PacketSinkHelper packetSinkHelper("ns3::UdpSocketFactory", sinkAddress);
    ApplicationContainer sinkApps = packetSinkHelper.Install(nodes.Get(1));
    sinkApps.Start(Seconds(0.0));
    sinkApps.Stop(Seconds(2500.0));

    Config::ConnectWithoutContext("/NodeList/0/ApplicationList/*/$ns3::PacketLengthModulation/Tx", MakeCallback(&TxCallback));
    Config::ConnectWithoutContext("/NodeList/1/ApplicationList/*/$ns3::PacketSink/Rx", MakeCallback(&RxCallback));

    /////////////////////////// Add background traffic///////////////////////
    NodeContainer bgNodes; 
    NodeContainer allNodes = NodeContainer(nodes, bgNodes);

    NetDeviceContainer bgDevices = wifi.Install(wifiPhy, wifiMac, bgNodes);

    mobility.Install(bgNodes);

    internet.Install(bgNodes);

    ipv4.NewNetwork();
    ipv4.Assign(bgDevices);

    // Read the CSV file
    std::vector<PacketInfo> packetInfos = ReadCsvFile("combined.csv"); // Ensure the path is correct and covers up to 600 seconds

    Address remoteAddressBackground(InetSocketAddress(Ipv4Address("255.255.255.255"), 8081)); // Broadcast address on port 8081

    for (uint32_t i = 0; i < bgNodes.GetN(); ++i)
    {
        ScheduleBackgroundTraffic(bgNodes.Get(i), packetInfos, remoteAddressBackground);
    }

    /////////////////////////// Add background traffic///////////////////////

    // Run the simulation
    Simulator::Stop(Seconds(2500.0)); // Extended simulation time to 10 minutes
    Simulator::Run();
    Simulator::Destroy();

    txStream.close();
    rxStream.close();
    if (decodedResultsStream.is_open())
    {
        decodedResultsStream.close();
    }

    return 0;
}
