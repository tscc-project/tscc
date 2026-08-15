interface Item244 { id: number; name: string; }
type Key244 = string | number;
export function make244(id: number, name: string): Item244 {
  const value: Item244 = {id, name};
  return value as Item244;
}
export const item244: Item244 = make244(244, "item-244");
