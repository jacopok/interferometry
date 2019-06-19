{
gSystem->Load("/usr/local/lib/libopencv_videoio.so");
gSystem->Load("/usr/local/lib/libopencv_highgui.so");
gSystem->Load("/usr/local/lib/libopencv_imgproc.so");
gROOT->ProcessLine(".L ANALISI/Point.C");
gROOT->ProcessLine(".L ANALISI/Analizer.C");
gROOT->ProcessLine(".L ANALISI/CenterManager.C");
gROOT->ProcessLine(".L GUI/MyGUI.C");
gROOT->ProcessLine(".L BIGGRAPH/BigGraph.C");
gROOT->ProcessLine(".L SpokePerson/Spokeperson.C");
gROOT->ProcessLine(".x gimli.C");
}
