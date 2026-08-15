namespace N41 {
  export const base: number = 41;
  export function add(x: number): number { return base + x; }
}
function over41(x: number): number;
function over41(x: string): string;
function over41(x: number | string): number | string { return x; }
abstract class A41 { abstract f(x: number): number; }
class B41 extends A41 { override f(x: number): number { return x + 1; } }
enum E41 { A = 5 / 2, B, C = 2 ** 3, D }
const angle41 = <number>(41 + 1);
const check41 = {x: angle41} satisfies {x: number};
console.log(N41.add(1), over41(41), new B41().f(1), E41.B, E41.D, check41.x);
