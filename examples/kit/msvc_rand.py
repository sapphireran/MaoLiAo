"""MSVC CRT rand / srand, used only for the world-3 catalog seeds."""


class MsvcRand:
    def __init__(self, seed: int = 1):
        self.hold = seed

    def srand(self, seed: int) -> None:
        self.hold = seed

    def rand(self) -> int:
        self.hold = self.hold * 214013 + 2531011
        self.hold = ((self.hold + 2**31) % 2**32) - 2**31
        return (self.hold & 0xFFFFFFFF) >> 16 & 0x7FFF

    def random_ab(self, a: int, b: int) -> int:
        """define.h: #define random(a,b) (rand()%(b-a)+a)"""
        return self.rand() % (b - a) + a
