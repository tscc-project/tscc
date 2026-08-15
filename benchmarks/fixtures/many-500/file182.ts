interface Item182 { id: number; name: string; }
type Key182 = string | number;
export function make182(id: number, name: string): Item182 {
  const value: Item182 = {id, name};
  return value as Item182;
}
export const item182: Item182 = make182(182, "item-182");
