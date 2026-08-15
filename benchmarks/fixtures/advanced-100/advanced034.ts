namespace N34 {
  export const base: number = 34;
  export function add(x: number): number { return base + x; }
}
function over34(x: number): number;
function over34(x: string): string;
function over34(x: number | string): number | string { return x; }
abstract class A34 { abstract f(x: number): number; }
class B34 extends A34 { override f(x: number): number { return x + 1; } }
enum E34 { A = 5 / 2, B, C = 2 ** 3, D }
const angle34 = <number>(34 + 1);
const check34 = {x: angle34} satisfies {x: number};
console.log(N34.add(1), over34(34), new B34().f(1), E34.B, E34.D, check34.x);
