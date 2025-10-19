# Tilting Maze Game (OpenGL)

기울기(tilting)로 공을 굴려 미로의 도착 지점에 도달하는 퍼즐 게임.  

OpenGL API 사용.

---

## 주요 기능
- 재귀 DFS로 미로 생성(2칸 단위 벽 허물기)
- 기울기 기반 물리(가속/마찰/벽 반사)
- BFS 최단경로 시각화(메뉴 → Find Path)
- 조명/재질 적용: 벽(큐브), 공(구), 바닥 경로 하이라이트

---
## 플레이 화면
<img width="1089" height="630" alt="image" src="https://github.com/user-attachments/assets/fa7cc295-4380-45f7-9aec-17babb531afa" />


---

## 조작
- 화살표: 테이블 기울기(좌/우, 상/하)
- **Q / E**: 카메라 거리 감소 / 증가
- **R / T**: 가속도 감소 / 증가
- **W / A / S / D**: 격자 한 칸 스냅 이동(벽 통과 불가, 속/가속 0)
- **우클릭 메뉴**: Reset Map / Find Path
- 클리어 시 2초 후 맵 리셋

---

## 빌드 환경
- Windows + Visual Studio(MSVC) **x64 권장**
- OpenGL 헤더(gl.h / glu.h): Windows SDK 포함
- GLUT: **FreeGLUT 필요**

### FreeGLUT 구성
- 헤더: `include/`  
  `freeglut.h`, `freeglut_std.h`, `freeglut_ext.h`, `glut.h`
- 라이브러리: `lib/freeglut.lib` (MSVC x64)
- DLL: 실행 폴더(`x64/Debug/` 등) → `freeglut.dll`

---

## 폴더 구조(예시)
```
TiltingMazeGame/
├─ README.md
├─ TiltingMazeGame.vcxproj
├─ src/
│ └─ TiltingMazeGame.cpp
├─ include/
│ └─ GL/
│ ├─ freeglut.h
│ ├─ freeglut_std.h
│ ├─ freeglut_ext.h
│ └─ glut.h
├─ lib/
│ └─ freeglut.lib
└─ x64/
└─ Debug/
├─ TiltingMazeGame.exe
└─ freeglut.dll
```
---
## 트러블 슈팅 케이스

