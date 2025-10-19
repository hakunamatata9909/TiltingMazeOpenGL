# Tilting Maze Game (OpenGL)

����(tilting)�� ���� ���� �̷��� ���� ������ �����ϴ� ���� ����.  

OpenGL API ���.

---

## �ֿ� ���
- ��� DFS�� �̷� ����(2ĭ ���� �� �㹰��)
- ���� ��� ����(����/����/�� �ݻ�)
- BFS �ִܰ�� �ð�ȭ(�޴� �� Find Path)
- ����/���� ����: ��(ť��), ��(��), �ٴ� ��� ���̶���Ʈ

---
## �÷��� ȭ��
|�̹���1|�̹���2|
|----|----|

---

## ����
- ȭ��ǥ: ���̺� ����(��/��, ��/��)
- **Q / E**: ī�޶� �Ÿ� ���� / ����
- **R / T**: ���ӵ� ���� / ����
- **W / A / S / D**: ���� �� ĭ ���� �̵�(�� ��� �Ұ�, ��/���� 0)
- **��Ŭ�� �޴�**: Reset Map / Find Path
- Ŭ���� �� 2�� �� �� ����

---

## ���� ȯ��
- Windows + Visual Studio(MSVC) **x64 ����**
- OpenGL ���(gl.h / glu.h): Windows SDK ����
- GLUT: **FreeGLUT �ʿ�**

### FreeGLUT ����
- ���: `include/`  
  `freeglut.h`, `freeglut_std.h`, `freeglut_ext.h`, `glut.h`
- ���̺귯��: `lib/freeglut.lib` (MSVC x64)
- DLL: ���� ����(`x64/Debug/` ��) �� `freeglut.dll`

---

## ���� ����(����)
```
TiltingMazeGame/
���� README.md
���� TiltingMazeGame.vcxproj
���� src/
�� ���� TiltingMazeGame.cpp
���� include/
�� ���� GL/
�� ���� freeglut.h
�� ���� freeglut_std.h
�� ���� freeglut_ext.h
�� ���� glut.h
���� lib/
�� ���� freeglut.lib
���� x64/
���� Debug/
���� TiltingMazeGame.exe
���� freeglut.dll
```
---
## Ʈ���� ���� ���̽�

