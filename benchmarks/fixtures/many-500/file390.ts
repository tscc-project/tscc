interface Item390 { id: number; name: string; }
type Key390 = string | number;
export function make390(id: number, name: string): Item390 {
  const value: Item390 = {id, name};
  return value as Item390;
}
export const item390: Item390 = make390(390, "item-390");
