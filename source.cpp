#include <ns3/core-module.h>
#include <ns3/point-to-point-module.h>
#include <ns3/internet-module.h>
#include <ns3/applications-module.h>

//Não copie
#include <ns3/netanim-module.h>
#include <ns3/bs-net-device.h>
#include <ns3/csma-module.h>
#include <ns3/uan-module.h>
using namespace ns3;
int main()
{
    //LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_ALL);
    //LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_ALL);
    //LogComponentEnable("LiIonEnergySource", LOG_LEVEL_DEBUG);
    LogComponentEnableAll(LOG_LEVEL_DEBUG);

    std::string outputFolder = "output/";
    NodeContainer nodes, nodesRede1, nodesRede2;
    nodes.Create(3);
    nodesRede1.Add(nodes.Get(0));
    nodesRede1.Add(nodes.Get(1));
    nodesRede2.Add(nodes.Get(1));
    nodesRede2.Add(nodes.Get(2));

    NetDeviceContainer netInterfacesRede1, netInterfacesRede2;
    PointToPointHelper pointHelper;
    pointHelper.SetChannelAttribute("Delay",StringValue("5ms"));
    netInterfacesRede1 = pointHelper.Install(nodesRede1);
    netInterfacesRede2 = pointHelper.Install(nodesRede2);


    InternetStackHelper internetHelper;
    internetHelper.Install(nodesRede1);
    internetHelper.Install(nodesRede2);
    //pointHelper.EnableAscii(outputFolder+"out.txt",nodesRede1);
    Ipv4InterfaceContainer interfaceRede1, interfaceRede2;
    Ipv4AddressHelper ipv4Helper;
    ipv4Helper.SetBase("192.168.1.0","255.255.255.0");
    interfaceRede1 = ipv4Helper.Assign(netInterfacesRede1);

    ipv4Helper.SetBase("192.168.2.1","255.255.255.0");
    interfaceRede2 = ipv4Helper.Assign(netInterfacesRede2);

    //Configurar roteador

    //Configurar aplicações
    ApplicationContainer clientApp, serverApp;

    UdpEchoServerHelper echoServerHelper(8080);
    serverApp = echoServerHelper.Install(nodesRede1.Get(0)); //Instala servidor echo em 192.168.1.0 ou 1

    UdpEchoClientHelper echoClientHelper(Ipv4Address("192.168.1.0"),8080);
    clientApp = echoClientHelper.Install(nodesRede2.Get(1)); //Instala cliente echo em 192.168.1.1 ou 2

    serverApp.Start(Seconds(1));
    serverApp.Stop(Seconds(10));
    clientApp.Start(Seconds(2));
    clientApp.Stop(Seconds(9));

    //A partir daqui, tem um servidor escutando na porta 8080 e um cliente mandando mensagens para ela


    //Não copie
    BaseStationNetDevice b;
    SubscriberStationNetDevice s;
    CsmaNetDevice c;
    UanNetDevice u;

    AnimationInterface anim(outputFolder+"anim2.xml");
    anim.SetMaxPktsPerTraceFile(0xFFFFFFFF);
    anim.EnablePacketMetadata(true);
    anim.EnableIpv4RouteTracking (outputFolder+"routingtable-wireless.xml", Seconds (0), Seconds (9), Seconds (0.25));

    //copiem
    Simulator::Stop(Seconds(10)); // Rodar simulação por 10 segundos
    Simulator::Run();
    Simulator::Destroy();

    return 0;
}