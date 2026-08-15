interface Item341 { id: number; name: string; }
type Key341 = string | number;
export function make341(id: number, name: string): Item341 {
  const value: Item341 = {id, name};
  return value as Item341;
}
export const item341: Item341 = make341(341, "item-341");
