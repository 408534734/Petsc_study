#include <petscmat.h>//�����;�����ص������ļ�

char help[] = "An example of basic useages of matrix in petsc."

int main(int argc, char* argv[]) {
	PetscInitialize(&argc, &args, (char*)0, help);
	Mat A;
	//����һ������ָ�룩
	MatCreate(PETSC_COMM_WORLD, &A);
	//��ʼ���������
	PetscInt rows = 16, cloumns = 16;
	MatSetSizes(A, PETSC_DECIDE, PETSC_DECIDE, rows, columns);
	/*���þ���Ĵ�С
	����1������
	����2������������
	����3������������
	����4��ȫ��������
	����5��ȫ��������
	���غ�ȫ���п�����һ����PETSC_DECIDE*/
	MatSetType(A, MATMPIAIJ);
	/*���þ�������Ϊ�ڶ���������ָ��������
	�����������У�
	MATSEQAIJ������ϡ�����
	MATMPIAIJ������ϡ�����
	MATAIJ��ϡ�����Ĭ��Ϊ����
	MATSEQDENSE�����г��ܾ���
	MATMPIDENSE�����г��ܾ���
	MATDENSE�����ܾ���Ĭ��Ϊ����
	*/
	//MatSetFromOptions(A);
	/*���������sҲ����������ʱ�ò���ָ��
	��ϸ��http://www.mcs.anl.gov/petsc/petsc-3.7/docs/manualpages/Mat/MatSetType.html#MatSetType*/
	return 0;
}