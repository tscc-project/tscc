namespace N1 {
  export const base: number = 1;
  export function add(x: number): number { return base + x; }
}
function over1(x: number): number;
function over1(x: string): string;
function over1(x: number | string): number | string { return x; }
abstract class A1 { abstract f(x: number): number; }
class B1 extends A1 { override f(x: number): number { return x + 1; } }
enum E1 { A = 5 / 2, B, C = 2 ** 3, D }
const angle1 = <number>(1 + 1);
const check1 = {x: angle1} satisfies {x: number};
console.log(N1.add(1), over1(1), new B1().f(1), E1.B, E1.D, check1.x);
