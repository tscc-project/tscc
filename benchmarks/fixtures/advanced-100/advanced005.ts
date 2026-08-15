namespace N5 {
  export const base: number = 5;
  export function add(x: number): number { return base + x; }
}
function over5(x: number): number;
function over5(x: string): string;
function over5(x: number | string): number | string { return x; }
abstract class A5 { abstract f(x: number): number; }
class B5 extends A5 { override f(x: number): number { return x + 1; } }
enum E5 { A = 5 / 2, B, C = 2 ** 3, D }
const angle5 = <number>(5 + 1);
const check5 = {x: angle5} satisfies {x: number};
console.log(N5.add(1), over5(5), new B5().f(1), E5.B, E5.D, check5.x);
