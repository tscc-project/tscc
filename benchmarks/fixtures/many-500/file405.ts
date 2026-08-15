interface Item405 { id: number; name: string; }
type Key405 = string | number;
export function make405(id: number, name: string): Item405 {
  const value: Item405 = {id, name};
  return value as Item405;
}
export const item405: Item405 = make405(405, "item-405");
