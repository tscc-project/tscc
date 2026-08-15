interface Item293 { id: number; name: string; }
type Key293 = string | number;
export function make293(id: number, name: string): Item293 {
  const value: Item293 = {id, name};
  return value as Item293;
}
export const item293: Item293 = make293(293, "item-293");
