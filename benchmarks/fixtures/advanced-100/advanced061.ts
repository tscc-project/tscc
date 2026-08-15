namespace N61 {
  export const base: number = 61;
  export function add(x: number): number { return base + x; }
}
function over61(x: number): number;
function over61(x: string): string;
function over61(x: number | string): number | string { return x; }
abstract class A61 { abstract f(x: number): number; }
class B61 extends A61 { override f(x: number): number { return x + 1; } }
enum E61 { A = 5 / 2, B, C = 2 ** 3, D }
const angle61 = <number>(61 + 1);
const check61 = {x: angle61} satisfies {x: number};
console.log(N61.add(1), over61(61), new B61().f(1), E61.B, E61.D, check61.x);
