namespace N10 {
  export const base: number = 10;
  export function add(x: number): number { return base + x; }
}
function over10(x: number): number;
function over10(x: string): string;
function over10(x: number | string): number | string { return x; }
abstract class A10 { abstract f(x: number): number; }
class B10 extends A10 { override f(x: number): number { return x + 1; } }
enum E10 { A = 5 / 2, B, C = 2 ** 3, D }
const angle10 = <number>(10 + 1);
const check10 = {x: angle10} satisfies {x: number};
console.log(N10.add(1), over10(10), new B10().f(1), E10.B, E10.D, check10.x);
