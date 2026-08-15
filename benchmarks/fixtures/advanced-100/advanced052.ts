namespace N52 {
  export const base: number = 52;
  export function add(x: number): number { return base + x; }
}
function over52(x: number): number;
function over52(x: string): string;
function over52(x: number | string): number | string { return x; }
abstract class A52 { abstract f(x: number): number; }
class B52 extends A52 { override f(x: number): number { return x + 1; } }
enum E52 { A = 5 / 2, B, C = 2 ** 3, D }
const angle52 = <number>(52 + 1);
const check52 = {x: angle52} satisfies {x: number};
console.log(N52.add(1), over52(52), new B52().f(1), E52.B, E52.D, check52.x);
