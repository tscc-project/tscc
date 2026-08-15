interface Item151 { id: number; name: string; }
type Key151 = string | number;
export function make151(id: number, name: string): Item151 {
  const value: Item151 = {id, name};
  return value as Item151;
}
export const item151: Item151 = make151(151, "item-151");
