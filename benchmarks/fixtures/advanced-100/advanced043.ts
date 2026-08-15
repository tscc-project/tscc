namespace N43 {
  export const base: number = 43;
  export function add(x: number): number { return base + x; }
}
function over43(x: number): number;
function over43(x: string): string;
function over43(x: number | string): number | string { return x; }
abstract class A43 { abstract f(x: number): number; }
class B43 extends A43 { override f(x: number): number { return x + 1; } }
enum E43 { A = 5 / 2, B, C = 2 ** 3, D }
const angle43 = <number>(43 + 1);
const check43 = {x: angle43} satisfies {x: number};
console.log(N43.add(1), over43(43), new B43().f(1), E43.B, E43.D, check43.x);
