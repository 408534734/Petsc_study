# 这里是有关vec和petsc的一些基础功能的使用

### ex2.c ：  
<ol>
    <li>petsc并行程序的创建</li>
    <li>向量的创建</li>
    <li>向量的整体赋值</li>
    <li>向量的通过处理器间通讯指定赋值</li>
    <li>赋值期间通讯问题</li>
</ol>

### ex3.c :
<ol>
    <li>画图同能，由于没有OpenGL环境，略</li>
    <li>只对本处理器上的向量进行赋值</li>
    <li>向量上的值的获取</li>
</ol>

### ex5.c :
<ol>
    <li>将向量数据输出到文件</li>
    <li>使用文件记录向量数据</li>
    <li>读取记录的向量数据</li>
    <li>自定义程序运行事件，并记录每个事件运行情况，在执行时加参数 -log_view 以输出查看运行日志</li>
</ol>

### ex6.c :  
<ol>
    <li>使用文件输入输出储存单个数据，如向量的大小值，但是要注意，使用 PetscBinaryWrite() 和 PetscBinaryRead() 并不是并行的，当读写大向量、矩阵的时候要使用 VecView() and VecLoad() 和 MatView() and MatLoad()</li>
    <li>将向量中用于储存的数组的指针拿出来，直接读写里面的值</li>
</ol>

### ex8.c :
<ol>
    <li>在每个处理器上重新建立（可以是不同的）序号访问映射，比如可以让序号0不再是访问整个向量的第一个元素，而是该向量在本处理器上的第一个元素</li>
</ol>

### ex9.c :
<ol>
    <li>使用冗余向量，缓存别的处理器上面的值到自己的处理器上面来</li>
    <li>以当前向量为原型，创建新的向量</li>
    <li>利用缓存输出，将每个处理器要输出的缓存，然后按处理器编号顺序输出</li>
</ol>

### ex10.c :
<ol>
    <li>给对象取名字，可以在VecView里面显示</li>
    <li>抛出错误并停止程序</li>
    <li>用二进制或者hdf5模式读写文件（此部分略）</li>
</ol>

### ex1.c : 向量的基本操作
<ol>
    <li>取最大最小值及其下标</li>
    <li>向量与向量相加</li>
    <li>向量乘标量、向量点乘向量、向量与向量对应元素相乘</li>
</ol>

### ex11.c : 
    向量的范数，才疏学浅，并不知道是啥，也没查到，不管了

### ex12.c : 
<ol>
    <li>将向量分块，这里把它叫做分块向量（英文是multi-component vector，虽然这样不是很准确）
        <ul>和它关联的有（但ex11.c中没有提及）：
            <li>VecSetValuesBlocked(Vec x,PetscInt ni,const PetscInt ix[],const PetscScalar y[],InsertMode iora)
            将一个已经分块的矩阵的某些块赋值为指定值
            参数1：要赋值的矩阵
            参数2：有几块要赋值
            参数3：哪几块要赋值
            参数4：要放入的值（按块在参数3中出现的顺序）
            参数5：赋值模式（代替或叠加）
            需要搭配VecAssemblyBegin(), VecAssemblyEnd()使用</li>
            <li>VecGetBlockSize(Vec v,PetscInt *bs)
            获取本矩阵块大小</li>
        </ul>
    <li>上面提到了怎么将整块赋值，也可以用非分块向量给分块向量中的每一块的第几个元素赋值</li>
</ol>

### ex15.c : 
    和Mathematical这个软件进行玩耍一下，略


### ex16.c :
<ol>
    <li>续ex12.c，当然也有将所有块内的第几个元素赋值为另一个向量里所有块第几个元素的值</li>
</ol>

### ex18.c :
<ol>
    <li>一个数值积分的例子</li>
</ol>
    

<meta http-equiv="refresh" content="0.2">