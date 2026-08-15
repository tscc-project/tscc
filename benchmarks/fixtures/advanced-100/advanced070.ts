namespace N70 {
  export const base: number = 70;
  export function add(x: number): number { return base + x; }
}
function over70(x: number): number;
function over70(x: string): string;
function over70(x: number | string): number | string { return x; }
abstract class A70 { abstract f(x: number): number; }
class B70 extends A70 { override f(x: number): number { return x + 1; } }
enum E70 { A = 5 / 2, B, C = 2 ** 3, D }
const angle70 = <number>(70 + 1);
const check70 = {x: angle70} satisfies {x: number};
console.log(N70.add(1), over70(70), new B70().f(1), E70.B, E70.D, check70.x);
