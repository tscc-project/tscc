namespace N84 {
  export const base: number = 84;
  export function add(x: number): number { return base + x; }
}
function over84(x: number): number;
function over84(x: string): string;
function over84(x: number | string): number | string { return x; }
abstract class A84 { abstract f(x: number): number; }
class B84 extends A84 { override f(x: number): number { return x + 1; } }
enum E84 { A = 5 / 2, B, C = 2 ** 3, D }
const angle84 = <number>(84 + 1);
const check84 = {x: angle84} satisfies {x: number};
console.log(N84.add(1), over84(84), new B84().f(1), E84.B, E84.D, check84.x);
