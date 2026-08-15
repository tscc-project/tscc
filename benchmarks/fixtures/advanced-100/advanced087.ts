namespace N87 {
  export const base: number = 87;
  export function add(x: number): number { return base + x; }
}
function over87(x: number): number;
function over87(x: string): string;
function over87(x: number | string): number | string { return x; }
abstract class A87 { abstract f(x: number): number; }
class B87 extends A87 { override f(x: number): number { return x + 1; } }
enum E87 { A = 5 / 2, B, C = 2 ** 3, D }
const angle87 = <number>(87 + 1);
const check87 = {x: angle87} satisfies {x: number};
console.log(N87.add(1), over87(87), new B87().f(1), E87.B, E87.D, check87.x);
