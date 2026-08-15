namespace N12 {
  export const base: number = 12;
  export function add(x: number): number { return base + x; }
}
function over12(x: number): number;
function over12(x: string): string;
function over12(x: number | string): number | string { return x; }
abstract class A12 { abstract f(x: number): number; }
class B12 extends A12 { override f(x: number): number { return x + 1; } }
enum E12 { A = 5 / 2, B, C = 2 ** 3, D }
const angle12 = <number>(12 + 1);
const check12 = {x: angle12} satisfies {x: number};
console.log(N12.add(1), over12(12), new B12().f(1), E12.B, E12.D, check12.x);
