namespace N0 {
  export const base: number = 0;
  export function add(x: number): number { return base + x; }
}
function over0(x: number): number;
function over0(x: string): string;
function over0(x: number | string): number | string { return x; }
abstract class A0 { abstract f(x: number): number; }
class B0 extends A0 { override f(x: number): number { return x + 1; } }
enum E0 { A = 5 / 2, B, C = 2 ** 3, D }
const angle0 = <number>(0 + 1);
const check0 = {x: angle0} satisfies {x: number};
console.log(N0.add(1), over0(0), new B0().f(1), E0.B, E0.D, check0.x);
