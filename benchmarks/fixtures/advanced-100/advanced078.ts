namespace N78 {
  export const base: number = 78;
  export function add(x: number): number { return base + x; }
}
function over78(x: number): number;
function over78(x: string): string;
function over78(x: number | string): number | string { return x; }
abstract class A78 { abstract f(x: number): number; }
class B78 extends A78 { override f(x: number): number { return x + 1; } }
enum E78 { A = 5 / 2, B, C = 2 ** 3, D }
const angle78 = <number>(78 + 1);
const check78 = {x: angle78} satisfies {x: number};
console.log(N78.add(1), over78(78), new B78().f(1), E78.B, E78.D, check78.x);
