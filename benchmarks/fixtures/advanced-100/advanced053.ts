namespace N53 {
  export const base: number = 53;
  export function add(x: number): number { return base + x; }
}
function over53(x: number): number;
function over53(x: string): string;
function over53(x: number | string): number | string { return x; }
abstract class A53 { abstract f(x: number): number; }
class B53 extends A53 { override f(x: number): number { return x + 1; } }
enum E53 { A = 5 / 2, B, C = 2 ** 3, D }
const angle53 = <number>(53 + 1);
const check53 = {x: angle53} satisfies {x: number};
console.log(N53.add(1), over53(53), new B53().f(1), E53.B, E53.D, check53.x);
