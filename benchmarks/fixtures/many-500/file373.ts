interface Item373 { id: number; name: string; }
type Key373 = string | number;
export function make373(id: number, name: string): Item373 {
  const value: Item373 = {id, name};
  return value as Item373;
}
export const item373: Item373 = make373(373, "item-373");
