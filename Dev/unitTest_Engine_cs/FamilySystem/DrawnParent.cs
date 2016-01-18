﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using asd;

namespace unitTest_Engine_cs.FamilySystem
{
    class DrawnParent : EngineTest
    {
        public DrawnParent() : base(60)
        {
        }

        protected override void OnStart()
        {
            var texture = Engine.Graphics.CreateTexture2D(CloudTexturePath);

            var other = new TextureObject2D()
            {
                Texture = texture,
                Color = new Color(255, 128, 128, 255),
                Position = new Vector2DF(0, 0),
                DrawingPriority = 2,
            };

            var parent = new TextureObject2D()
            {
                Texture = texture,
                Color = new Color(128, 128, 128, 255),
                Position = new Vector2DF(100, 0),
                DrawingPriority = 3,
            };

            var child1 = new TextureObject2D()
            {
                Texture = texture,
                Color = new Color(0, 255, 255, 255),
                Position = new Vector2DF(0, 100),
            };

            var child2 = new TextureObject2D()
            {
                Texture = texture,
                Color = new Color(255, 255, 0, 255),
                Position = new Vector2DF(-100, 100),
            };

            parent.AddDrawnChild(child1,
                ChildManagementMode.Nothing,
                ChildTransformingMode.Position,
                ChildDrawingMode.Color);
            parent.AddDrawnChild(child2,
                ChildManagementMode.Nothing,
                ChildTransformingMode.Position,
                ChildDrawingMode.Color | ChildDrawingMode.DrawingPriority);

            Engine.AddObject2D(other);
            Engine.AddObject2D(parent);
            Engine.AddObject2D(child1);
            Engine.AddObject2D(child2);
            Console.WriteLine("other:" + other.AbsoluteDrawingPriority);
            Console.WriteLine("parent:" + parent.AbsoluteDrawingPriority);
            Console.WriteLine("child1:" + child1.AbsoluteDrawingPriority);
            Console.WriteLine("child2:" + child2.AbsoluteDrawingPriority);
        }
    }
}
