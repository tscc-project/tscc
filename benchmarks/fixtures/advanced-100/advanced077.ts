namespace N77 {
  export const base: number = 77;
  export function add(x: number): number { return base + x; }
}
function over77(x: number): number;
function over77(x: string): string;
function over77(x: number | string): number | string { return x; }
abstract class A77 { abstract f(x: number): number; }
class B77 extends A77 { override f(x: number): number { return x + 1; } }
enum E77 { A = 5 / 2, B, C = 2 ** 3, D }
const angle77 = <number>(77 + 1);
const check77 = {x: angle77} satisfies {x: number};
console.log(N77.add(1), over77(77), new B77().f(1), E77.B, E77.D, check77.x);
