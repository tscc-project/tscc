namespace N60 {
  export const base: number = 60;
  export function add(x: number): number { return base + x; }
}
function over60(x: number): number;
function over60(x: string): string;
function over60(x: number | string): number | string { return x; }
abstract class A60 { abstract f(x: number): number; }
class B60 extends A60 { override f(x: number): number { return x + 1; } }
enum E60 { A = 5 / 2, B, C = 2 ** 3, D }
const angle60 = <number>(60 + 1);
const check60 = {x: angle60} satisfies {x: number};
console.log(N60.add(1), over60(60), new B60().f(1), E60.B, E60.D, check60.x);
