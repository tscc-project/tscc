namespace N15 {
  export const base: number = 15;
  export function add(x: number): number { return base + x; }
}
function over15(x: number): number;
function over15(x: string): string;
function over15(x: number | string): number | string { return x; }
abstract class A15 { abstract f(x: number): number; }
class B15 extends A15 { override f(x: number): number { return x + 1; } }
enum E15 { A = 5 / 2, B, C = 2 ** 3, D }
const angle15 = <number>(15 + 1);
const check15 = {x: angle15} satisfies {x: number};
console.log(N15.add(1), over15(15), new B15().f(1), E15.B, E15.D, check15.x);
