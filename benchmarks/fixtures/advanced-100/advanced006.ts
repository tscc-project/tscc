namespace N6 {
  export const base: number = 6;
  export function add(x: number): number { return base + x; }
}
function over6(x: number): number;
function over6(x: string): string;
function over6(x: number | string): number | string { return x; }
abstract class A6 { abstract f(x: number): number; }
class B6 extends A6 { override f(x: number): number { return x + 1; } }
enum E6 { A = 5 / 2, B, C = 2 ** 3, D }
const angle6 = <number>(6 + 1);
const check6 = {x: angle6} satisfies {x: number};
console.log(N6.add(1), over6(6), new B6().f(1), E6.B, E6.D, check6.x);
