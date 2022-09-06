## A5. Ray Tracing

![A5.bmp](https://raw.githubusercontent.com/mboath/ComputerGraphics2020Fall/main/images/A5.bmp)

* 光线跟踪算法的实现

1. 光线与球体、平面、矩形、圆柱的求交与交点纹理坐标的获取
2. 不同光照：点光源、矩形光源、球形光源
3. 不同材质
4. 多线程

---

## A6. Terrain Engine

![A6.png](https://raw.githubusercontent.com/mboath/ComputerGraphics2020Fall/main/images/A6.png)

* 真实可探索的三维场景绘制

1. 摄像机坐标系与全局坐标系之间的变换
2. 地形的读取、绘制及细节纹理贴图
3. 天空和地形的倒影效果
4. 海面的波浪效果

Demo Video: https://github.com/mboath/ComputerGraphics2020Fall/blob/main/A6_Terrain%20Engine/A6.mp4

---

## B1. Spiral Starts

![B1.jpg](https://raw.githubusercontent.com/mboath/ComputerGraphics2020Fall/main/images/B1.jpg)

* 在二维平面上绘制出星星旋转的效果

1. 星星沿阿基米德螺线绕中心旋转并不断向外圈扩散
2. 星星的颜色不同

---

## B2. Orbiting Planets

![B2.jpg](https://raw.githubusercontent.com/mboath/ComputerGraphics2020Fall/main/images/B2.jpg)

* 在二维平面上模拟行星围绕日运动

1. 根据数据确定行星的运行轨道与大小
2. 分别绘制轨道、行星与文字
3. 键盘交互控制旋转速度

---

## B5. Snowing

![B5.jpg](https://raw.githubusercontent.com/mboath/ComputerGraphics2020Fall/main/images/B5.jpg)

* 在三维空间中实现简单的粒子效果-下雪

1. 雪花的生成位置、下落速度和大小不同
2. 单位时间内生成雪花随时间增多
3. 雪花落地即消失

---

## C1. Moving Triangles and Squares

![C1.jpg](https://raw.githubusercontent.com/mboath/ComputerGraphics2020Fall/main/images/C1.jpg)

* 在二维平面上绘制三角形和四边形并着色（flat/smooth），同时旋转和平移

---

## C2. PHONG

![C2.png](https://raw.githubusercontent.com/mboath/ComputerGraphics2020Fall/main/images/C2.png)

* 在三维空间中绘制一个旋转的彩色四棱锥并添加光照效果

1. PHONG光照模型
2. 可以通过控制物体对光的反射材质因子和不同光的因子达到不同的光照效果

---

## C3. Texture Mapping

![C3.jpg](https://raw.githubusercontent.com/mboath/ComputerGraphics2020Fall/main/images/C3.jpg)

* 在三维空间中绘制一个旋转的正方体并加载纹理

1. 使用三种纹理过滤方式加载纹理（线性采样、 mipmap 和最近点采样）

2. 键盘交互控制控制纹理过滤方式的切换