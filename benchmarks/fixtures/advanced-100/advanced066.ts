namespace N66 {
  export const base: number = 66;
  export function add(x: number): number { return base + x; }
}
function over66(x: number): number;
function over66(x: string): string;
function over66(x: number | string): number | string { return x; }
abstract class A66 { abstract f(x: number): number; }
class B66 extends A66 { override f(x: number): number { return x + 1; } }
enum E66 { A = 5 / 2, B, C = 2 ** 3, D }
const angle66 = <number>(66 + 1);
const check66 = {x: angle66} satisfies {x: number};
console.log(N66.add(1), over66(66), new B66().f(1), E66.B, E66.D, check66.x);
