/*
  This uses the low level PetscBinaryWrite() and PetscBinaryRead() to access a binary file. It will not work in parallel!
  We HIGHLY recommend using instead VecView() and VecLoad() to read and write Vectors in binary format (which also work in parallel). Then you can use
  share/petsc/matlab/PetscBinaryRead() and share/petsc/matlab/PetscBinaryWrite() to read (or write) the vector into MATLAB.
  Note this also works for matrices with MatView() and MatLoad().
*/


static char help[] = "Writes an array to a file, then reads an array from a file, then forms a vector.\n\n";

#include <petscvec.h>

#undef __FUNCT__
#define __FUNCT__ "main"
int main(int argc,char **args)
{
  PetscErrorCode ierr;
  PetscMPIInt    size;
  int            fd;
  PetscInt       i,m = 10,sz;
  PetscScalar    *avec,*array;
  Vec            vec;
  PetscViewer    view_out,view_in;
  //文件读写器

  PetscInitialize(&argc,&args,(char*)0,help);
  ierr = MPI_Comm_size(PETSC_COMM_WORLD,&size);CHKERRQ(ierr);
  if (size != 1) SETERRQ(PETSC_COMM_SELF,1,"This is a uniprocessor example only!");

  ierr = PetscOptionsGetInt(NULL,NULL,"-m",&m,NULL);CHKERRQ(ierr);

  /* ---------------------------------------------------------------------- */
  /*          PART 1: Write some data to a file in binary format            */
  /* ---------------------------------------------------------------------- */

  /* Allocate array and set values */
  ierr = PetscMalloc1(m,&array);CHKERRQ(ierr);
  for (i=0; i<m; i++) array[i] = i*10.0;
	//创建一个一会要放入文件的向量
  /* Open viewer for binary output */
  ierr = PetscViewerBinaryOpen(PETSC_COMM_SELF,"input.dat",FILE_MODE_WRITE,&view_out);CHKERRQ(ierr);
  ierr = PetscViewerBinaryGetDescriptor(view_out,&fd);CHKERRQ(ierr);
  //获得输出流
  /* Write binary output */
  ierr = PetscBinaryWrite(fd,&m,1,PETSC_INT,PETSC_FALSE);CHKERRQ(ierr);
  //将数组的大小写入文件
  /*
	 参数1：读入流
	 参数2：要写入文件数据的指针
	 参数3：要写入数据的个数
	 参数4：要写入数据的数据类型
	 参数5：指针所指向的内容是否需要被保留
   */
  ierr = PetscBinaryWrite(fd,array,m,PETSC_SCALAR,PETSC_FALSE);CHKERRQ(ierr);
  //将数组的每个元素写入文件
  /* Destroy the output viewer and work array */
  ierr = PetscViewerDestroy(&view_out);CHKERRQ(ierr);
  ierr = PetscFree(array);CHKERRQ(ierr);
  //释放数组内存
  /* ---------------------------------------------------------------------- */
  /*          PART 2: Read data from file and form a vector                 */
  /* ---------------------------------------------------------------------- */

  /* Open input binary viewer */
  ierr = PetscViewerBinaryOpen(PETSC_COMM_SELF,"input.dat",FILE_MODE_READ,&view_in);CHKERRQ(ierr);
  ierr = PetscViewerBinaryGetDescriptor(view_in,&fd);CHKERRQ(ierr);
  //获得输入流
  /* Create vector and get pointer to data space */
  ierr = VecCreate(PETSC_COMM_SELF,&vec);CHKERRQ(ierr);
  ierr = VecSetSizes(vec,PETSC_DECIDE,m);CHKERRQ(ierr);
  //这个sample这里不科学，应该用后面的sz来确定size
  ierr = VecSetFromOptions(vec);CHKERRQ(ierr);
  ierr = VecGetArray(vec,&avec);CHKERRQ(ierr);

  /* Read data into vector */
  ierr = PetscBinaryRead(fd,&sz,1,PETSC_INT);CHKERRQ(ierr);
  //读入数组的大小
  if (sz <=0) SETERRQ(PETSC_COMM_SELF,1,"Error: Must have array length > 0");

  ierr = PetscPrintf(PETSC_COMM_SELF,"reading data in binary from input.dat, sz =%D ...\n",sz);CHKERRQ(ierr);
  ierr = PetscBinaryRead(fd,avec,sz,PETSC_SCALAR);CHKERRQ(ierr);
  //读入数组
  /*
	 参数1：输入流
	 参数2：要放入的数组
	 参数3：读取的数据个数
	 参数4：要读取的数据类型
   */
  /* View vector */
  ierr = VecRestoreArray(vec,&avec);CHKERRQ(ierr);
  ierr = VecView(vec,PETSC_VIEWER_STDOUT_SELF);CHKERRQ(ierr);

  /* Free data structures */
  ierr = VecDestroy(&vec);CHKERRQ(ierr);
  ierr = PetscViewerDestroy(&view_in);CHKERRQ(ierr);
  ierr = PetscFinalize();
  return 0;
}
