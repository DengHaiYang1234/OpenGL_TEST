#ifndef BITMAP_CPP
#define BITMAP_CPP
#include <iostream>
#include "hyMath.cpp"
using namespace std;

class Bitmap
{
    /*
uint32_t color：是指将原有的RGBA是个通道的颜色，通过每8位进行保存，刚好组成1个int整形。
(PS:当前颜色的RGBA其实每个通道都是8位也就是1个字节，总共也就是4个字节，用int直接可以表示，毕竟省内存，因为可以替换掉之前的RGBA的四维向量表示法)
*/

private:
    int32_t _w;     //宽
    int32_t _h;     //高
    int32_t _pitch; //一行数据所占的字节数，或者说是跨度 https://blog.csdn.net/a102111/article/details/9326785
    uint8_t *_bits; //bitmap总字节数

    struct BITMAPINFOHEADER
    {
        uint32_t biSize;
        uint32_t biWidth;
        int32_t biHeight;
        uint16_t biPlanes;
        uint16_t biBitCount;
        uint32_t biCompression;
        uint32_t biSizeImage;
        uint32_t biXPelsPerMeter;
        uint32_t biYPelsPerMeter;
        uint32_t biClrUsed;
        uint32_t biClrImportant;
    };

    int GetW() const { return _w; }
    int GetH() const { return _h; }
    int GetPitch() const { return _pitch; }
    uint8_t *GetBits() { return _bits; }                               //首地址
    const uint8_t *GetBits() const { return _bits; }                   //首地址
    uint8_t *GetLine(int y) { return _bits + _pitch * y; }             //每行的首地址
    const uint8_t *GetLine(int y) const { return _bits + _pitch * y; } //每行的首地址

public:
    ~Bitmap() //析构函数
    {
        if (_bits)
            delete[] _bits;
        _bits = NULL;
    }

    Bitmap(int width, int height) : _w(width), _h(height)
    {
        _pitch = width * 4;               //一行数据所占的字节数 4表示行中的每个元素所占的字节数
        _bits = new uint8_t[_pitch * _h]; //总的字节数
    }

    //统一填充bitmap所有元素的颜色
    void Fill(uint32_t color)
    {
        for (int j = 0; j < _h; j++)
        {
            //确定行的首地址
            uint32_t *row = (uint32_t *)(_bits + j * _pitch);
            //地址依次递增 每次递增增加4个字节32位
            for (int i = 0; i < _w; i++, row++)
            {
                //获取颜色指针对应的值，并将32的值拷贝至每行每个元素的32位地址
                memcpy(row, &color, sizeof(uint32_t));
            }
        }
    }

    void SetPixel(int x, int y, uint32_t color)
    {
        if (x >= 0 && x < _w && y >= 0 && y < _h)
        {
            //确定当前行列所指的地址，填充32位的颜色
            memcpy(_bits + y * _pitch + x * 4, &color, sizeof(uint32_t));
        }
    }

    void SetPixel(int x, int y, const Vec4f &color)
    {
        SetPixel(x, y, vector_to_color(color));
    }

    uint32_t GetPixel(int x, int y) const
    {
        uint32_t color = 0;
        if (x >= 0 && x < _w && y >= 0 && y < _h)
        {
            //确定当前行列所指的地址，获取地址存储的颜色
            memcpy(&color, _bits + y * _pitch + x * 4, sizeof(uint32_t));
        }
        return color;
    }

    void DrawLine(int x1, int y1, int x2, int y2, uint32_t color)
    {
        int x, y;
        //同一个点不做处理
        if (x1 == x2 && y1 == y2)
        {
            SetPixel(x1, y1, color);
            return;
        }
        else if (x1 == x2) //x相同,向上开始填充
        {
            int inc = (y1 <= y2) ? 1 : -1;
            for (y = y1; y != y2; y += inc)
            {
                SetPixel(x1, y, color);
            }

            SetPixel(x2, y2, color);
        }
        else if (y1 == y2)
        {
            int inc = (x1 <= x2) ? 1 : -1;
            for (x = x1; x != x2; x += inc)
            {
                SetPixel(x, y1, color);
            }

            SetPixel(x2, y2, color);
        }
        else
        {
            //DDA 斜截式
            // DrawLineWithDDA(x1, y1, x2, y2, color);
            //中心点
            // DrawLineWithMidpoint(x1, y1, x2, y2, color);
            // //指定t
            // DrawLineWithPointOblique(x1, y1, x2, y2, color);
            // //横向插值
            // DrawLineWithHorizontalLerp(x1,y1,x2,y2,color);
            // //长边插值
            // DrawLineWithLongSideLerp(x1,y1,x2,y2,color);
            // //Bresenham斜率判断
            // DrawLineWithBresenham(x1,y1,x2,y2,color);
            // //Bresenham斜率优化判断
            DrawLineWithOptimizeBresenham(x1, y1, x2, y2, color);
        }
    }

    //测试点与直线方程的位置
    void TestPointWithLinearEquation(int x1, int y1, int x2, int y2)
    {
        //当斜率<=0时，直线方程值<0表示在直线的上方 >0表示在直线的下方
        //当斜率>0时，直线方程值>0表示在直线的上方 <0表示在直线的下方
        cout << "x1:" << x1 << ",y1:" << y1 << ",x2:" << x2 << ",y2:" << y2 << endl;
        int cx = x1;
        int cy = y1 + 2;
        int E01 = -(cx - x1) * (y2 - y1) + (cy - y1) * (x2 - x1);
        cout << "位于y1上方：:" << E01 << endl;
        cx = x1;
        cy = y1 - 2;
        E01 = -(cx - x1) * (y2 - y1) + (cy - y1) * (x2 - x1);
        cout << "位于y1下方:" << E01 << endl;
        //每次递增x的方式显然不适合斜率过大的直线。斜率过大会导致屏幕上显示的点少而且稀疏，甚至并不能在屏幕上显示出人可以辨识出的直线形状
        //局限：斜率为负数的时候，需要重新适配
        float k = (y2 - y1) * 1.0 / (x2 - x1);
        cout << "k:" << k << endl;
    }

    //⭐️直线算法核心思想在于利用增量算法
    //主要两篇文章推荐
    //https://zhuanlan.zhihu.com/p/20213658
    //https://zhuanlan.zhihu.com/p/128927381

    //DDA 斜截式
    void DrawLineWithDDA(int x1, int y1, int x2, int y2, uint32_t color)
    {
        //每次递增x的方式显然不适合斜率过大的直线。斜率过大会导致屏幕上显示的点少而且稀疏，甚至并不能在屏幕上显示出人可以辨识出的直线形状
        //局限：斜率为负数的时候，需要重新适配
        float k = (y2 - y1) * 1.0 / (x2 - x1);
        int y = y1;
        for (int x = x1; x <= x2; ++x)
        {
            SetPixel(x, y, color);
            y += k;
        }
    }

    //点斜式划线
    void DrawLineWithPointOblique(int x1, int y1, int x2, int y2, uint32_t color, float dnsity = 0.003)
    {
        //自定义斜率t 用点斜式画线 但取决于间隔t的密度来决定线的精细
        for (float t = 0.; t < 1.; t += dnsity)
        {
            int x = x1 + (x2 - x1) * t;
            int y = y1 + (y2 - y1) * t;
            SetPixel(x, y, color);
        }
    }

    //横向插值
    void DrawLineWithHorizontalLerp(int x1, int y1, int x2, int y2, uint32_t color)
    {

        //获取横向两点间的距离与总距离的比值 作为纵向的插值运算的t
        for (int x = x1; x < x2; x++)
        {
            float t = (x - x1) / (float)(x2 - x1);
            int y = y1 * (1.0f - t) + t * y2;
            SetPixel(x, y, color);
        }
    }

    //始终根据长边进行插值运算
    void DrawLineWithLongSideLerp(int x1, int y1, int x2, int y2, uint32_t color)
    {
        bool reverse = false;
        if (abs(x1 - x2) < abs(y1 - y2)) //始终根据长距离来计算插值t
        {
            swap(x1, y1);
            swap(x2, y2);
            reverse = true;
        }

        if (x1 > x2) //从左到右开始划线
        {
            swap(x1, x2);
            swap(y1, y2);
        }

        for (int x = x1; x <= x2; x++)
        {
            float t = (x - x1) / (float)(x2 - x1);
            int y = y1 * (1.0f - t) + t * y2;
            if (reverse)
            {
                SetPixel(y, x, color);
            }
            else
            {
                SetPixel(x, y, color);
            }
        }
    }

    //中点画线法  直线的一般式
    void DrawLineWithMidpoint(int x1, int y1, int x2, int y2, uint32_t color)
    {
        //这里显示不正确是因为用的opengl坐标系，左上为原点
        //https://zhuanlan.zhihu.com/p/65125995
        /*
        ⭐️中点画线算法总结
        1.下一个点的y值始终根据上一个点的结果来迭代的，也就是增量算法.
        F(x,y) = kx - y + b
        d0 = 0
        F(x1,y1) = F(x+1,y+0.5) = k(x+1) - (y+0.5) + b = kx - y + b + k - 0.5 = F(x,y) + k - 0.5;
        d1 = d0 + k - 0.5 = k - 0.5  
        F(x2,y2) = F(x1+1,y1+0.5) = k(x1+1) - (y1+0.5) + b = kx1 - y1 + b + k - 0.5 = F(x1,y1) + k - 0.5;
        d2 = d1 + k - 0.5 若d1 > 0  d1 = k - 1 若d1 <=0   d1 = k
        2.下一个点的选择根据是，先计算下一个点的中点，也就是y + 0.5的直线方程与直线的方向；
        若k>0,并且y+0.5位于直线的上方,说明直线位于中点的下方,画点的时候就选择不上移，(x,y)->(x+1,y);
        反之若y+0.5位于直线的下方，说明直线位于中点的上方,画点的时候就选择上移，(x,y)->(x+1,y);
        */
        bool reverse = false;
        //默认x方向为最长边
        if (abs(x1 - x2) < abs(y1 - y2))
        {
            swap(x1, y1);
            swap(x2, y2);
            reverse = true;
        }

        if (x1 > x2) //从左到右开始划线
        {
            swap(x1, x2);
            swap(y1, y2);
        }

        float k = (y2 - y1) * 1.0 / (x2 - x1);
        float d = 0;

        if (k > 0) //当斜率>0时，直线方程值>0表示在直线的上方 <0表示在直线的下方
        {
            int y = y1;
            for (int x = x1; x <= x2; ++x)
            {
                if (reverse)
                {
                    SetPixel(y, x, color);
                }
                else
                {
                    SetPixel(x, y, color);
                }

                //下一个点的计算
                if (d < 0)
                {
                    d += k;
                }
                else
                {
                    d += (k - 1);
                    y += 1;
                }
            }
        }
        else //当斜率<=0时，直线方程值<0表示在直线的上方 >0表示在直线的下方
        {
            //计算d的直线方程x+1改为x-1
            int y = y1;
            for (int x = x1; x <= x2; ++x)
            {
                if (reverse)
                {
                    SetPixel(y, x, color);
                }
                else
                {
                    SetPixel(x, y, color);
                }

                //k<- 点与直线的方向
                if (d < 0)
                {
                    d -= -(k + 1);
                    y -= 1;
                }
                else
                {
                    d -= -k;
                }
            }
        }
    }

    //Bresenham划线法  上下楼梯算法
    /*
        ⭐️⭐️⭐️⭐️Bresenham核心算法总结:
        由中心画线算法可得，如果将方程累加的d改为当前点的k值,若k>下一个点的中点，就上移，反之下移
    */
    void DrawLineWithBresenham(int x1, int y1, int x2, int y2, uint32_t color)
    {
        bool reverse = false;
        //默认x方向为最长边
        if (abs(x1 - x2) < abs(y1 - y2))
        {
            swap(x1, y1);
            swap(x2, y2);
            reverse = true;
        }

        if (x1 > x2) //从左到右开始划线
        {
            swap(x1, x2);
            swap(y1, y2);
        }

        int dx = x2 - x1;
        int dy = y2 - y1;

        //两点式的斜率公式   https://zhuanlan.zhihu.com/p/26263309
        float k = abs(dy / (float)dx);
        float d = 0;
        int y = y1;

        for (int x = x1; x <= x2; x++)
        {
            if (reverse)
            {
                SetPixel(y, x, color);
            }
            else
            {
                SetPixel(x, y, color);
            }

            //https://zhuanlan.zhihu.com/p/65466579
            //我们x移动一部，y方向上相应的移动一个k值，d也就累加相应的k值
            //当（d>0.5)时我们的y就上移
            //当（d<0.5)时我们的y就不上移
            d += k;

            if (d > 0.5f)
            {
                y += (y2 > y1 ? 1 : -1);
                d -= 1; //重置初始
            }
        }
    }

    uint32_t SampleBilinear(float x, float y) const
    {
        int32_t fx = (int32_t)(x * 0x10000);
        int32_t fy = (int32_t)(y * 0x10000);
        int32_t x1 = Clamp(0, _w - 1, fx >> 16);
        int32_t y1 = Clamp(0, _h - 1, fy >> 16);
        int32_t x2 = Clamp(0, _w - 1, x1 + 1);
        int32_t y2 = Clamp(0, _h - 1, y1 + 1);
        int32_t dx = (fx >> 8) & 0xff;
        int32_t dy = (fy >> 8) & 0xff;
        if (_w <= 0 || _h <= 0)
            return 0;
        uint32_t c00 = GetPixel(x1, y1);
        uint32_t c01 = GetPixel(x2, y1);
        uint32_t c10 = GetPixel(x1, y2);
        uint32_t c11 = GetPixel(x2, y2);
        return BilinearInterp(c00, c01, c10, c11, dx, dy);
    }

        // 纹理采样：直接传入 Vec2f
    Vec4f Sample2D(const Vec2f &uv)  const
    {
        return Sample2D(uv.x, uv.y);
    }

    Vec4f Sample2D(float u, float v)  const
    {
        uint32_t rgba = SampleBilinear(u * _w + 0.5f, v * _h + 0.5f);
        return vector_from_color(rgba);
    }



    //优化的Bresenham划线法 float转int 这样是为了优化浮点除法，优化性能
    void DrawLineWithOptimizeBresenham(int x1, int y1, int x2, int y2, uint32_t color)
    {
        bool reverse = false;
        if (abs(x1 - x2) < abs(y1 - y2))
        {
            swap(x1, y1);
            swap(x2, y2);
            reverse = true;
        }

        if (x1 > x2) //从左到右开始划线
        {
            swap(x1, x2);
            swap(y1, y2);
        }

        //长边
        int dx = x2 - x1;
        //短边
        int dy = y2 - y1;
        float k = abs(dy) * 2;
        float d = 0;
        int y = y1;

        for (int x = x1; x <= x2; x++)
        {
            if (reverse)
            {
                SetPixel(y, x, color);
            }
            else
            {
                SetPixel(x, y, color);
            }

            //https://zhuanlan.zhihu.com/p/65466579
            d += k;
            //d = (d + dy / dx) * 2 > 1
            //2 * (d*dx + dy) / dx > 1
            //2 * d * dx + 2 * dy > dx

            if (d > dx) //短边的值大于长边的值时，就上
            {
                y += (y2 > y1 ? 1 : -1);
                d -= dx * 2;
            }
        }
    }

    bool SaveFile(const char *filename, bool withAlpha = false) const //不允许对类中的成员变量进行修改
    {
        FILE *fp = fopen(filename, "wb");
        if (fp == NULL)
            return false;

        BITMAPINFOHEADER info;
        uint32_t pixelSize = (withAlpha) ? 4 : 3;         //是否有a通道
        uint32_t pitch = (GetW() * pixelSize + 3) & (~3); // ~3 = -4  &操作符
        info.biSizeImage = pitch * GetH();
        uint32_t bfSize = 54 + info.biSizeImage;
        uint32_t zero = 0, offset = 54;
        fputc(0x42, fp);
        fputc(0x4d, fp);
        fwrite(&bfSize, 4, 1, fp);
        fwrite(&zero, 4, 1, fp);
        fwrite(&offset, 4, 1, fp);
        info.biSize = 40;
        info.biWidth = GetW();
        info.biHeight = GetH();
        info.biPlanes = 1;
        info.biBitCount = (withAlpha) ? 32 : 24;
        info.biCompression = 0;
        info.biXPelsPerMeter = 0xb12;
        info.biYPelsPerMeter = 0xb12;
        info.biClrUsed = 0;
        info.biClrImportant = 0;
        fwrite(&info, sizeof(info), 1, fp);

        for (int y = 0; y < GetH(); y++)
        {
            const uint8_t *line = GetLine(info.biHeight - 1 - y);
            uint32_t padding = pitch - GetW() * pixelSize;
            for (int x = 0; x < GetW(); x++, line += 4)
            {
                fwrite(line, pixelSize, 1, fp);
            }
            for (int i = 0; i < (int)padding; i++)
            {
                fputc(0, fp);
            }
        }
        fclose(fp);
        return true;
    }
};

#endif
