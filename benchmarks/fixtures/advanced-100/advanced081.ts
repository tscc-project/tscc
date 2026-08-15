namespace N81 {
  export const base: number = 81;
  export function add(x: number): number { return base + x; }
}
function over81(x: number): number;
function over81(x: string): string;
function over81(x: number | string): number | string { return x; }
abstract class A81 { abstract f(x: number): number; }
class B81 extends A81 { override f(x: number): number { return x + 1; } }
enum E81 { A = 5 / 2, B, C = 2 ** 3, D }
const angle81 = <number>(81 + 1);
const check81 = {x: angle81} satisfies {x: number};
console.log(N81.add(1), over81(81), new B81().f(1), E81.B, E81.D, check81.x);
