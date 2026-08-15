interface Item415 { id: number; name: string; }
type Key415 = string | number;
export function make415(id: number, name: string): Item415 {
  const value: Item415 = {id, name};
  return value as Item415;
}
export const item415: Item415 = make415(415, "item-415");
