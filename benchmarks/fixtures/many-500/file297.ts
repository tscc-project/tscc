interface Item297 { id: number; name: string; }
type Key297 = string | number;
export function make297(id: number, name: string): Item297 {
  const value: Item297 = {id, name};
  return value as Item297;
}
export const item297: Item297 = make297(297, "item-297");
