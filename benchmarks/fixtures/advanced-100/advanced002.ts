namespace N2 {
  export const base: number = 2;
  export function add(x: number): number { return base + x; }
}
function over2(x: number): number;
function over2(x: string): string;
function over2(x: number | string): number | string { return x; }
abstract class A2 { abstract f(x: number): number; }
class B2 extends A2 { override f(x: number): number { return x + 1; } }
enum E2 { A = 5 / 2, B, C = 2 ** 3, D }
const angle2 = <number>(2 + 1);
const check2 = {x: angle2} satisfies {x: number};
console.log(N2.add(1), over2(2), new B2().f(1), E2.B, E2.D, check2.x);
