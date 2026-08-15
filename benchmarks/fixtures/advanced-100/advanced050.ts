namespace N50 {
  export const base: number = 50;
  export function add(x: number): number { return base + x; }
}
function over50(x: number): number;
function over50(x: string): string;
function over50(x: number | string): number | string { return x; }
abstract class A50 { abstract f(x: number): number; }
class B50 extends A50 { override f(x: number): number { return x + 1; } }
enum E50 { A = 5 / 2, B, C = 2 ** 3, D }
const angle50 = <number>(50 + 1);
const check50 = {x: angle50} satisfies {x: number};
console.log(N50.add(1), over50(50), new B50().f(1), E50.B, E50.D, check50.x);
