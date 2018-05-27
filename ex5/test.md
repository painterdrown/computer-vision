# 测试报告

## 迭代法

### 最坏的 10 张结果

<img src='img/dataset/gt/12.jpg' style='width:40%;height:40%'/>

<img src='img/dataset/gt/18.jpg' style='width:40%;height:40%'/>

<img src='img/dataset/gt/19.jpg' style='width:40%;height:40%'/>

<img src='img/dataset/gt/20.jpg' style='width:40%;height:40%'/>

<img src='img/dataset/gt/22.jpg' style='width:40%;height:40%'/>

<img src='img/dataset/gt/79.jpg' style='width:40%;height:40%'/>

<img src='img/dataset/gt/80.jpg' style='width:40%;height:40%'/>

<img src='img/dataset/gt/81.jpg' style='width:40%;height:40%'/>

<img src='img/dataset/gt/97.jpg' style='width:40%;height:40%'/>

<img src='img/dataset/gt/98.jpg' style='width:40%;height:40%'/>

### 最好的 10 张结果

要选出最好的 10 张有点难度，前 10 张我看效果都不错，不妨就它们了！

<img src='img/dataset/gt/1.jpg' style='width:40%;height:40%'/>

<img src='img/dataset/gt/2.jpg' style='width:40%;height:40%'/>

<img src='img/dataset/gt/3.jpg' style='width:40%;height:40%'/>

<img src='img/dataset/gt/4.jpg' style='width:40%;height:40%'/>

<img src='img/dataset/gt/5.jpg' style='width:40%;height:40%'/>

<img src='img/dataset/gt/6.jpg' style='width:40%;height:40%'/>

<img src='img/dataset/gt/7.jpg' style='width:40%;height:40%'/>

<img src='img/dataset/gt/8.jpg' style='width:40%;height:40%'/>

<img src='img/dataset/gt/9.jpg' style='width:40%;height:40%'/>

<img src='img/dataset/gt/10.jpg' style='width:40%;height:40%'/>

### 性能分析

根据下面的核心代码以及前 5 张图片（剩下的 95 张情况类似）的运行日志可以看出：

平均每张图片只需进行 5 个迭代就可以结束，因此十分高效（速度是 Otsu 法的 50 倍），分割的效果挺不错（但是有少数图片不行）。

```sh
********************** processing image 1 **********************
interation 0: G1_count = 12055676, G2_count = 137092, T = 22.000000, new_T = 68.080635, delta = 46.080635
interation 1: G1_count = 20719804, G2_count = 3665732, T = 68.080635, new_T = 97.450523, delta = 29.369888
interation 2: G1_count = 28631089, G2_count = 7947215, T = 97.450523, new_T = 102.550323, delta = 5.099800
********************** processing image 2 **********************
interation 0: G1_count = 12167638, G2_count = 25130, T = 22.000000, new_T = 86.540115, delta = 64.540115
interation 1: G1_count = 20349194, G2_count = 4036342, T = 86.540115, new_T = 126.687950, delta = 40.147835
interation 2: G1_count = 28434375, G2_count = 8143929, T = 126.687950, new_T = 105.195084, delta = 21.492867
interation 3: G1_count = 36569139, G2_count = 12201933, T = 105.195084, new_T = 89.061951, delta = 16.133133
interation 4: G1_count = 44738814, G2_count = 16225026, T = 89.061951, new_T = 78.609818, delta = 10.452133
interation 5: G1_count = 53029355, G2_count = 20127253, T = 78.609818, new_T = 68.858421, delta = 9.751396
********************** processing image 3 **********************
interation 0: G1_count = 12069701, G2_count = 123067, T = 22.000000, new_T = 88.056732, delta = 66.056732
interation 1: G1_count = 20895452, G2_count = 3490084, T = 88.056732, new_T = 129.060303, delta = 41.003571
interation 2: G1_count = 29631515, G2_count = 6946789, T = 129.060303, new_T = 100.611084, delta = 28.449219
interation 3: G1_count = 38431757, G2_count = 10339315, T = 100.611084, new_T = 84.685043, delta = 15.926041
interation 4: G1_count = 47278440, G2_count = 13685400, T = 84.685043, new_T = 74.558945, delta = 10.126099
interation 5: G1_count = 56406475, G2_count = 16750133, T = 74.558945, new_T = 67.363884, delta = 7.195061
********************** processing image 4 **********************
interation 0: G1_count = 12187753, G2_count = 5015, T = 22.000000, new_T = 65.568436, delta = 43.568436
interation 1: G1_count = 20740075, G2_count = 3645461, T = 65.568436, new_T = 95.244705, delta = 29.676270
interation 2: G1_count = 28778743, G2_count = 7799561, T = 95.244705, new_T = 102.233513, delta = 6.988808
********************** processing image 5 **********************
interation 0: G1_count = 12037944, G2_count = 154824, T = 22.000000, new_T = 89.642715, delta = 67.642715
interation 1: G1_count = 21327440, G2_count = 3058096, T = 89.642715, new_T = 125.640190, delta = 35.997475
interation 2: G1_count = 30475041, G2_count = 6103263, T = 125.640190, new_T = 96.475769, delta = 29.164421
interation 3: G1_count = 39742114, G2_count = 9028958, T = 96.475769, new_T = 80.002182, delta = 16.473587
interation 4: G1_count = 49106867, G2_count = 11856973, T = 80.002182, new_T = 69.830322, delta = 10.171860
interation 5: G1_count = 58783228, G2_count = 14373380, T = 69.830322, new_T = 62.848969, delta = 6.981354
```

```C++
do {
	cimg_forXY(gray, x, y) {
		if (gray.atXY(x, y) > T) {
			gt.atXY(x, y) = 255.0f;
			G1_intensity += gray.atXY(x, y);
			++G1_count;
		} else {
			gt.atXY(x, y) = 0.0f;
			G2_intensity += gray.atXY(x, y);
			++G2_count;
		}
	}
	float m1 = G1_intensity / G1_count;
	float m2 = G2_intensity / G2_count;
	new_T = (m1 + m2) / 2;
	delta = abs(new_T - T);
	printf("interation %d: G1_count = %d, G2_count = %d, T = %f, new_T = %f, delta = %f\n", interation, G1_count, G2_count, T, new_T, delta);
	T = new_T;
	interation++;
} while (delta > T_DELTA);
```

## Otsu 法

### 最坏的 10 张结果

<img src='img/dataset/otsu/12.jpg' style='width:40%;height:40%'/>

<img src='img/dataset/otsu/18.jpg' style='width:40%;height:40%'/>

<img src='img/dataset/otsu/19.jpg' style='width:40%;height:40%'/>

<img src='img/dataset/otsu/20.jpg' style='width:40%;height:40%'/>

<img src='img/dataset/otsu/22.jpg' style='width:40%;height:40%'/>

<img src='img/dataset/otsu/79.jpg' style='width:40%;height:40%'/>

<img src='img/dataset/otsu/80.jpg' style='width:40%;height:40%'/>

<img src='img/dataset/otsu/81.jpg' style='width:40%;height:40%'/>

<img src='img/dataset/otsu/97.jpg' style='width:40%;height:40%'/>

<img src='img/dataset/otsu/98.jpg' style='width:40%;height:40%'/>

### 最好的 10 张结果

<img src='img/dataset/otsu/1.jpg' style='width:40%;height:40%'/>

<img src='img/dataset/otsu/2.jpg' style='width:40%;height:40%'/>

<img src='img/dataset/otsu/3.jpg' style='width:40%;height:40%'/>

<img src='img/dataset/otsu/4.jpg' style='width:40%;height:40%'/>

<img src='img/dataset/otsu/5.jpg' style='width:40%;height:40%'/>

<img src='img/dataset/otsu/6.jpg' style='width:40%;height:40%'/>

<img src='img/dataset/otsu/7.jpg' style='width:40%;height:40%'/>

<img src='img/dataset/otsu/8.jpg' style='width:40%;height:40%'/>

<img src='img/dataset/otsu/9.jpg' style='width:40%;height:40%'/>

<img src='img/dataset/otsu/10.jpg' style='width:40%;height:40%'/>

### 性能分析

速度上相比于迭代法慢了很多，因为为了找到最优阈值，要遍历所以的灰度。但是分割的效果比迭代法好，几乎所以图片都能有比较好的前后景分割。

```C++
for (int t = 0; t <= L; ++t) {
	int foreground_count = 0, background_count = 0;
	float foreground_gray = 0.0f, background_gray = 0.0f;
	cimg_forXY(gray, x, y) {
		if (gray.atXY(x, y) > t) {
			foreground_gray += gray.atXY(x, y);
			++foreground_count;
		} else {
			background_gray += gray.atXY(x, y);
			++background_count;
		}
	}
	float w0 = (float)foreground_count / (float)all_count;
	float w1 = (float)background_count / (float)all_count;
	float u0 = foreground_gray / foreground_count;
	float u1 = background_gray / background_count;
	float g = w0 * w1 * pow(u0 - u1, 2);
	if (g > g_max) {
		t_max = t;
		g_max = g;
		printf("better threshold found: t = %d, g = %f\n", t, g);
	}
}
```
