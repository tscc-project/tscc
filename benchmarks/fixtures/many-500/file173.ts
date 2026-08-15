interface Item173 { id: number; name: string; }
type Key173 = string | number;
export function make173(id: number, name: string): Item173 {
  const value: Item173 = {id, name};
  return value as Item173;
}
export const item173: Item173 = make173(173, "item-173");
