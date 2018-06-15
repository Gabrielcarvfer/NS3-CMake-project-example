#include <ns3/core-module.h>
#include <ns3/point-to-point-module.h>
#include <ns3/internet-module.h>
#include <ns3/applications-module.h>
#include <ns3/wifi-module.h>
#include <ns3/mobility-module.h>
#include <ns3/csma-layout-module.h>

//Para netanim
#include <ns3/netanim-module.h>
#include <ns3/bs-net-device.h>
#include <ns3/csma-module.h>
#include <ns3/uan-module.h>

using namespace ns3;

void setup_mobility2(NodeContainer * nodes, std::string mobilityModel, double x, double y, double z, double radius)
{
    MobilityHelper mobility;

    Ptr<RandomBoxPositionAllocator> randomPositionAlloc = CreateObject<RandomBoxPositionAllocator> ();
    Ptr<UniformRandomVariable> xVal = CreateObject<UniformRandomVariable> ();
    xVal->SetAttribute ("Min", DoubleValue (x-radius));
    xVal->SetAttribute ("Max", DoubleValue (x+radius));
    Ptr<UniformRandomVariable> yVal = CreateObject<UniformRandomVariable> ();
    yVal->SetAttribute ("Min", DoubleValue (y-radius));
    yVal->SetAttribute ("Max", DoubleValue (y+radius));
    Ptr<UniformRandomVariable> zVal = CreateObject<UniformRandomVariable> ();
    zVal->SetAttribute ("Min", DoubleValue (z));
    zVal->SetAttribute ("Max", DoubleValue (z));

    mobility.SetPositionAllocator("ns3::RandomBoxPositionAllocator",
                                  "X", PointerValue (xVal),
                                  "Y", PointerValue (yVal),
                                  "Z", PointerValue (zVal)
    );
    mobility.SetMobilityModel(mobilityModel);
    mobility.Install(*nodes);
}

//Simple network setup
int main()
{

    std::string outputFolder = "output/";

    NodeContainer internet;
    internet.Create(10);
    NetDeviceContainer internetDev;
    CsmaHelper csmaHelper;


    for (unsigned i = 0; i < 9; i++)
        for (unsigned j = 1; j < 10; j++) {
            if (i == j)
                continue;
            NodeContainer umContainer;
            umContainer.Add(internet.Get(i));
            umContainer.Add(internet.Get(j));
            internetDev.Add(csmaHelper.Install(umContainer));
        }


    //Cria 5 subredes para cada dominio
    NodeContainer subredesLan1[5];
    NodeContainer subredesLan2[5];
    NetDeviceContainer subredesLan1Dev[5];
    NetDeviceContainer subredesLan2Dev[5];

    NodeContainer router1, router2;
    NodeContainer switches1, switches2;
    router1.Create(1);
    router2.Create(2);
    switches1.Add(router1);
    switches2.Add(router2);


    //Cria 6 nos por subrede para cada dominio
    for (unsigned i = 0; i<5; i++)
    {
        subredesLan1[i].Create(6);
        subredesLan2[i].Create(6);

        subredesLan1Dev[i] = csmaHelper.Install(subredesLan1[i]);
        subredesLan2Dev[i] = csmaHelper.Install(subredesLan2[i]);

        switches1.Add(subredesLan1[i].Get(0));
        switches2.Add(subredesLan2[i].Get(0));
    }

    NetDeviceContainer switches1Dev, switches2Dev;
    for (unsigned i = 1 ; i < 5; i++)
    {

        NodeContainer umContainer;
        umContainer.Add(switches1.Get(0));
        umContainer.Add(switches1.Get(i));
        NodeContainer outroContainer;

        outroContainer.Add(switches2.Get(0));
        outroContainer.Add(switches2.Get(i));
        switches1Dev.Add(csmaHelper.Install(umContainer));
        switches2Dev.Add(csmaHelper.Install(outroContainer));
    }

    NetDeviceContainer DominioWanDev;

    NodeContainer umContainer;
    umContainer.Add(router1.Get(0));
    umContainer.Add(internet.Get(0));
    NodeContainer outroContainer;
    outroContainer.Add(router2.Get(0));
    outroContainer.Add(internet.Get(5));
    DominioWanDev.Add(csmaHelper.Install(umContainer));
    DominioWanDev.Add(csmaHelper.Install(outroContainer));

    NodeContainer todosNos;
    todosNos.Add(internet);
    todosNos.Add(router1);
    todosNos.Add(router2);
    for (unsigned i=0; i < 5; i++)
    {
        todosNos.Add(subredesLan1[i]);
        todosNos.Add(subredesLan2[i]);
    }

    InternetStackHelper internetStackHelper;
    internetStackHelper.Install(todosNos);

    std::stringstream ss;

    Ipv4AddressHelper ipv4Helper;
    unsigned subredeNum = 1;
    for (unsigned i = 0 ; i < 5; i++)
    {
        ss << "192.168." << subredeNum << ".0";
        ipv4Helper.SetBase(ss.str().c_str(), "255.255.255.0");
        ipv4Helper.Assign(subredesLan1Dev[i]);
        ipv4Helper.NewNetwork();
        subredeNum++;
    }

    for (unsigned i = 0 ; i < 5; i++)
    {
        ss << "192.168." << subredeNum << ".0";
        ipv4Helper.SetBase(ss.str().c_str(), "255.255.255.0");
        ipv4Helper.Assign(subredesLan2Dev[i]);
        ipv4Helper.NewNetwork();
        subredeNum++;
    }

    ss << "192.168." << subredeNum << ".0";
    ipv4Helper.SetBase(ss.str().c_str(), "255.255.255.0");
    ipv4Helper.Assign(switches1Dev);
    ipv4Helper.NewNetwork();
    subredeNum++;

    ss << "192.168." << subredeNum << ".0";
    ipv4Helper.SetBase(ss.str().c_str(), "255.255.255.0");
    ipv4Helper.Assign(switches2Dev);
    ipv4Helper.NewNetwork();
    subredeNum++;

    ss << "192.168." << subredeNum << ".0";
    ipv4Helper.SetBase(ss.str().c_str(), "255.255.255.0");
    ipv4Helper.Assign(DominioWanDev);
    ipv4Helper.NewNetwork();
    subredeNum++;

    ss << "192.168." << subredeNum << ".0";
    ipv4Helper.SetBase(ss.str().c_str(), "255.255.255.0");
    ipv4Helper.Assign(internetDev);
    ipv4Helper.NewNetwork();
    subredeNum++;

    for (unsigned i = 0 ; i < 5; i++)
    {
        setup_mobility2(&subredesLan1[i], "ns3::ConstantPositionMobilityModel", 50+50*i, 100, 0, 20);
        setup_mobility2(&subredesLan2[i], "ns3::ConstantPositionMobilityModel", 50+50*i, 300, 0, 20);
    }
    setup_mobility2(&router1, "ns3::ConstantPositionMobilityModel", 250, 150, 0, 20);
    setup_mobility2(&router2, "ns3::ConstantPositionMobilityModel", 250, 250, 0, 20);
    setup_mobility2(&internet, "ns3::ConstantPositionMobilityModel",250, 200,0,20);


    csmaHelper.EnablePcapAll("csmapcap");
    //Configurar aplicações
    ApplicationContainer clientApp, serverApp;

    UdpEchoServerHelper echoServerHelper(8080);
    serverApp = echoServerHelper.Install(subredesLan1[0].Get(1)); //Instala servidor echo em 192.168.1.0 ou 1

    UdpEchoClientHelper echoClientHelper(Ipv4Address("192.168.1.2"),8080);
    echoClientHelper.SetAttribute("Interval",TimeValue(MilliSeconds(10)));
    clientApp = echoClientHelper.Install(subredesLan2[0].Get(1)); //Instala cliente echo em 192.168.1.1 ou 2

    serverApp.Start(Seconds(1));
    serverApp.Stop(Seconds(10));
    clientApp.Start(Seconds(2));
    clientApp.Stop(Seconds(9));

    //A partir daqui, tem um servidor escutando na porta 8080 e um cliente mandando mensagens para ela
    Ipv4GlobalRoutingHelper::PopulateRoutingTables ();


    //Exportar animação para o Netanim
    BaseStationNetDevice b;
    SubscriberStationNetDevice s;
    CsmaNetDevice c;
    UanNetDevice u;

    AnimationInterface anim(outputFolder+"anim2.xml");
    //anim.SetMaxPktsPerTraceFile(0xFFFFFFFF);
    //anim.EnablePacketMetadata(true);
    //anim.EnableIpv4RouteTracking (outputFolder+"routingtable-wireless.xml", Seconds (0), Seconds (9), Seconds (0.25));

    //Rodar o simulador
    Simulator::Stop(Minutes(2)); // Rodar simulação por 10 segundos
    Simulator::Run();
    Simulator::Destroy();

    return 0;
}