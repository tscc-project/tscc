interface Item464 { id: number; name: string; }
type Key464 = string | number;
export function make464(id: number, name: string): Item464 {
  const value: Item464 = {id, name};
  return value as Item464;
}
export const item464: Item464 = make464(464, "item-464");
