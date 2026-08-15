interface Item299 { id: number; name: string; }
type Key299 = string | number;
export function make299(id: number, name: string): Item299 {
  const value: Item299 = {id, name};
  return value as Item299;
}
export const item299: Item299 = make299(299, "item-299");
