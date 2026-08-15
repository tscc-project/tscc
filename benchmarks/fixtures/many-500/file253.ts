interface Item253 { id: number; name: string; }
type Key253 = string | number;
export function make253(id: number, name: string): Item253 {
  const value: Item253 = {id, name};
  return value as Item253;
}
export const item253: Item253 = make253(253, "item-253");
