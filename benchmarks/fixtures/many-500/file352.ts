interface Item352 { id: number; name: string; }
type Key352 = string | number;
export function make352(id: number, name: string): Item352 {
  const value: Item352 = {id, name};
  return value as Item352;
}
export const item352: Item352 = make352(352, "item-352");
