﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Reflection;

class Program
{
    static void Main(string[] args)
    {
        ISample[] samples =
			{
#if __CS__
			new Window_External(),
#endif
			new ImagePackageUI_Basic(),
				new ImagePackageUI_AlphaBlend(),
				new ImagePackageUI_Component(),

				new Pause_Basic(),

				new CustomPostEffect_Invert(),
				new CustomPostEffect_Mosaic(),

                new MapObject2D_Basic(),
                new MapObject2D_Camera(),

				new Action2D_Camera(),

				new DrawAdditionally2D_Material(),
            };

        var browser = new SampleBrowser(samples);
        browser.Run();
    }
}
