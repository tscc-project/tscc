interface Item324 { id: number; name: string; }
type Key324 = string | number;
export function make324(id: number, name: string): Item324 {
  const value: Item324 = {id, name};
  return value as Item324;
}
export const item324: Item324 = make324(324, "item-324");
